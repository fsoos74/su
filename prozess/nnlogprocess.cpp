#include "nnlogprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>
#include<fstream>
#include<limits>
#include<QFile>

#include "matrix.h"
#include "nn.h"
#include "xnlwriter.h"

/*
template<class Iter>
void norm( Iter begin, Iter end, std::pair<double, double> r, const double& NULL_VALUE=std::numeric_limits<double>::max()){

    if( r.first==r.second ) return;
    std::for_each( begin, end,
        [r, NULL_VALUE](double& x){if(x!=NULL_VALUE) x=(x-r.first)/(r.second-r.first);});
}

template<class Iter>
void unnorm( Iter begin, Iter end, std::pair<double, double> r, const double& NULL_VALUE=std::numeric_limits<double>::max()){

    if( r.first==r.second ) return;
    std::for_each( begin, end,
        [r, NULL_VALUE](double& x){if(x!=NULL_VALUE) x=r.first + x*(r.second-r.first);});
}
*/


std::ostream& operator<<( std::ostream& os, const NN& nn){

    os<<"NN:"<<std::endl;
    for( size_t i=1; i<nn.layer_count(); i++){
        os<<"layer "<<i<<std::endl;
        os<<"weights:"<<std::endl<<nn.weights(i)<<std::endl;
        os<<"biases:"<<std::endl<<nn.biases(i)<<std::endl;
    }

    return os;
}

NNLogProcess::NNLogProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("NN Log Interpolation"), project, parent){

}

ProjectProcess::ResultCode NNLogProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_outputName=getParam(parameters, "output-log");
        //m_unit=getParam( parameters, "unit");
        //m_descr=getParam(parameters, "description");

        m_well=getParam(parameters, "well");

        m_predictedName=getParam(parameters, "predicted-log");

        int i=0;
        while(1){
           QString name=QString("input-log-%1").arg(++i);
           if( parameters.contains(name)){
               auto l=parameters.value(name);
               m_inputNames.push_back(l);
           }
           else{
               break;
           }
        }

        m_hiddenNeurons=getParam( parameters, "hidden-neurons").toUInt();

        m_trainingRatio=getParam(parameters, "training-ratio").toDouble();
        m_trainingEpochs=getParam(parameters, "training-epochs").toUInt();
        m_learningRate=getParam(parameters, "learning-rate").toDouble();

        m_paramFileName=getParam( parameters, "param-file");
        m_historyFileName=getParam( parameters, "history-file");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode NNLogProcess::run(){

    auto ninputs=m_inputNames.size();

    // load input
    std::vector<std::shared_ptr<Log>> inputLogs(ninputs);

    for( int i=0; i<ninputs; i++){
        auto name=m_inputNames[i];
        auto l=project()->loadLog(m_well, name);
        if( !l ){
            setErrorString( tr("Loading log %1-%2 failed!").arg(m_well,name));
            return ResultCode::Error;
        }
        inputLogs[i]=l;
    }

    std::shared_ptr<Log> predictedLog=project()->loadLog(m_well, m_predictedName);
    if( !predictedLog ){
        setErrorString( tr("Loading log %1-%2 failed!").arg(m_well,m_predictedName));
        return ResultCode::Error;
    }

    // allocate output
    auto outputLog=std::make_shared<Log>( m_outputName.toStdString(), "","",//m_unit.toStdString(), m_descr.toStdString(),
                                       predictedLog->z0(), predictedLog->dz(), predictedLog->nz() );

    if( !outputLog){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }


    // prepare training input and results

    // make list of non null input and result samples for training
    std::vector<int> s_good_training;
    for( int i=0; i<predictedLog->nz(); i++){
        size_t good=0;
        for( auto l : inputLogs){
            if( (*l)[i]!=l->NULL_VALUE ) good++;
        }
        if( good<inputLogs.size()) continue;                                // at least one log has NULL value for this sample, skip
        if( (*predictedLog)[i]==predictedLog->NULL_VALUE ) continue;        // predicted log has NULL value for this sample, skip
        s_good_training.push_back(i);                                       // finally found a usuable sample, store it
    }

    // build training input and result matrices
    // randomly select training ratio good samples
    int ntraining=static_cast<int>( std::round(m_trainingRatio * s_good_training.size()));
    if( ntraining<=0 ){
        setErrorString("Not enought training data!!!");
        return ResultCode::Error;
    }

    random_shuffle( s_good_training.begin(), s_good_training.end());

    // finally build training matrices
    Matrix<double> X(ntraining, inputLogs.size());
    Matrix<double> Y(ntraining,1);

    for( int it=0; it<ntraining;it++){

        auto i=s_good_training[it];

        std::cout<<"it="<<it<<" i="<<i<<" [ ";
        for( size_t j=0; j<inputLogs.size(); j++){
            auto x=(*inputLogs[j])[i];
            X(it,j)=x;
            std::cout<<x<<" ";
        }
        std::cout<<"] -> ";
        auto y=(*predictedLog)[i];
        Y(it,0)=y;
        std::cout<<y<<std::endl;
    }


    // determine normalization params on training data, training data has no NULLs
   std::vector< std::pair<double,double> > Xmm(X.columns());
   for( unsigned j = 0; j<X.columns(); j++){
       auto itmm = std::minmax_element( X.column_begin(j), X.column_end(j));
       Xmm[j]=std::make_pair( *itmm.first, *itmm.second );
   }
   auto itmm=std::minmax_element( Y.begin(), Y.end() );
   auto Ymm=std::make_pair(*itmm.first, *itmm.second );

   // normalize training data
   for( unsigned j=0; j<X.columns(); j++){
       norm( X.column_begin(j), X.column_end(j), Xmm[j], inputLogs[j]->NULL_VALUE);
   }
   norm( Y.begin(), Y.end(), Ymm, predictedLog->NULL_VALUE );



    //create NN
    NN nn{ inputLogs.size(), m_hiddenNeurons, 1};      // XXX

    nn.setSigma(leaky_relu);
    nn.setSigmaPrime(leaky_relu_prime);
    nn.setEta(m_learningRate);


    // train nn
    //nn.train(X,Y,m_trainingEpochs);
    std::vector<double> hist;
    train(nn,X,Y,0.5,m_trainingEpochs,3,[&hist](size_t i, double err){hist.push_back(err);});


    // dump nn
    std::cout<<nn<<std::endl;

    // save nn params if requested
    if( !m_paramFileName.isEmpty()){
        QFile file(m_paramFileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            setErrorString(tr("Cannot write file %1:\n%2.")
                                 .arg(m_paramFileName)
                                 .arg(file.errorString()));
            return ResultCode::Error;
        }

        XNLWriter xnl(nn, m_inputNames, Xmm, Ymm);
        if( !xnl.writeFile(&file)){
            setErrorString("Writing Params failed!");
            return ResultCode::Error;
        }
    }

    // save history if requested
    if( !m_historyFileName.isEmpty()){
        std::ofstream os(m_historyFileName.toStdString().c_str());
        if(!os.good()){
            setErrorString(tr("Cannot write file %1.")
                                 .arg(m_historyFileName));
            return ResultCode::Error;
        }
        for( size_t i=0; i<hist.size(); i++){
            os<<"epoch= "<<i<<" error= "<<hist[i]<<std::endl;
        }
    }

    // compute interpolated data

    // norm input logs same as training data
    for( size_t j=0; j<inputLogs.size(); j++){
        norm( inputLogs[j]->begin(), inputLogs[j]->end(), Xmm[j], inputLogs[j]->NULL_VALUE);
    }

    Matrix<double> x(1, inputLogs.size());
    Matrix<double> y( 1, 1);
    for( size_t i=0; i<static_cast<size_t>(outputLog->nz()); i++){

        // check if all logs are not NULL at sample i
        int good=0;

        for( size_t j=0; j<inputLogs.size(); j++){
            auto v=(*(inputLogs[j]))[i];
            x(0,j)=v;
            if( v!=inputLogs[j]->NULL_VALUE ) good++;
        }

        if( good==ninputs){
            y = nn.feed_forward(x);
            (*outputLog)[i]=y(0,0);
        }
        else{
            (*outputLog)[i]=outputLog->NULL_VALUE;
        }

    }

    // scale output to reference predicted log
    unnorm( outputLog->begin(), outputLog->end(), Ymm, outputLog->NULL_VALUE); // XXX

    // savde output log
    if( !project()->addLog(m_well, m_outputName, *outputLog)){
        setErrorString("Adding result log to project failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
