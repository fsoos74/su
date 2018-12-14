#include "nnloginterpolationprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>

#include <QFile>

#include "xmlpreader.h"
#include "matrix.h"
#include "simplemlp.h"

/*
template<class Iter>
void norm( Iter begin, Iter end, std::pair<double, double> r, const double& NULL_VALUE){

    if( r.first==r.second ) return;
    std::for_each( begin, end,
        [r, NULL_VALUE](double& x){if( x!=NULL_VALUE) x=(x-r.first)/(r.second-r.first);});
}

template<class Iter>
void unnorm( Iter begin, Iter end, std::pair<double, double> r, const double& NULL_VALUE){

    if( r.first==r.second ) return;
    std::for_each( begin, end,
        [r, NULL_VALUE](double& x){if(x!=NULL_VALUE) x=r.first + x*(r.second-r.first);});
}
*/


NNLogInterpolationProcess::NNLogInterpolationProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("NN Log Interpolation"), project, parent){

}

ProjectProcess::ResultCode NNLogInterpolationProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_outputName=getParam(parameters, "output-log");
        m_wells=unpackParamList( getParam(parameters, "wells") );
        m_inputFile=getParam(parameters, "input-file");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode NNLogInterpolationProcess::run(){

    QFile file(m_inputFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        setErrorString( tr("Cannot read file %1:\n%2.")
                             .arg(m_inputFile)
                             .arg(file.errorString()));
        return ResultCode::Error;
    }

    int ilaper, xlaper;
    XMLPReader reader(m_mlp, m_inames, ilaper, xlaper);
    if (!reader.read(&file)) {
        setErrorString( tr("Parse error in file %1:\n\n%2")
                             .arg(m_inputFile)
                             .arg(reader.errorString()));
        return ResultCode::Error;
    }

    emit started(m_wells.size());
    qApp->processEvents();
    for( int i=0; i<m_wells.size(); i++){

        auto well = m_wells[i];
        auto res=processWell(well);
        if( res!=ResultCode::Ok ) return res;
        emit progress(i+1);
        qApp->processEvents();
    }

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}


ProjectProcess::ResultCode NNLogInterpolationProcess::processWell(QString well){

    // load input
   std::vector<std::shared_ptr<Log>> inputLogs;

   for( int i=0; i<m_inames.size(); i++){
       auto name=m_inames[i];
       auto l=project()->loadLog(well, name);
       if( !l ){
           setErrorString( tr("Loading log %1-%2 failed!").arg(well,name));
           return ResultCode::Error;
       }
       inputLogs.push_back(l);
   }

   // allocate output
   const Log& ref=*inputLogs.front();
   auto outputLog=std::make_shared<Log>( m_outputName.toStdString(), "","",//m_unit.toStdString(), m_descr.toStdString(),
                                      ref.z0(), ref.dz(), ref.nz() );

   if( !outputLog){
       setErrorString("Allocating log failed!");
       return ResultCode::Error;
   }

   Matrix<double> x(1, inputLogs.size());
   Matrix<double> y( 1, 1);
   for( size_t i=0; i<outputLog->nz(); i++){

       bool ok=true;
       for( size_t j=0; j<inputLogs.size(); j++){
           auto v=(*(inputLogs[j]))[i];
           if( v==inputLogs[j]->NULL_VALUE ){
               ok=false;
               break;
           }
           x(0,j)=v;
       }

       y = m_mlp.predict(x);
       std::cout<<x<<" -> "<<y<<std::endl;
       (*outputLog)[i]=y(0,0);
       //emit progress(it);
       //qApp->processEvents();
   }

   //emit currentTask("Saving output...");
   //emit started(1);
   //emit progress(0);
   //qApp->processEvents();

   // scale output log to original range
   /*if( outputRange.first!=outputRange.second){
       auto& r=outputRange;
       std::for_each( outputLog->begin(), outputLog->end(), [r](double& x){ x= r.first + x*(r.second-r.first);});
       //std::for_each( outputLog->begin(), outputLog->end(), [r](double& x){ x= (x+1)*(r.second-r.first)/2;}); // -1..1
   }
*/
   if( !project()->addLog(well, m_outputName, *outputLog)){
       setErrorString("Adding result log to project failed!");
       return ResultCode::Error;
   }

    return ResultCode::Ok;
}
