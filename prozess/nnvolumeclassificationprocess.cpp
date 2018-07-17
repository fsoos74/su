#include "nnvolumeclassificationprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <QApplication>
#include<iostream>

#include <QFile>

#include "xnlreader.h"
#include "matrix.h"
#include "nn.h"

const int CHUNK_ILINES=1;

NNVolumeClassificationProcess::NNVolumeClassificationProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("NN Volume Classification"), project, parent){

}

ProjectProcess::ResultCode NNVolumeClassificationProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_outputName=getParam(parameters, "output-volume");
        m_inputFile=getParam(parameters, "input-file");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode NNVolumeClassificationProcess::run(){

    // setup nn
    QFile file(m_inputFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        setErrorString( tr("Cannot read file %1:\n%2.")
                             .arg(m_inputFile)
                             .arg(file.errorString()));
        return ResultCode::Error;
    }

    XNLReader reader(m_nn, m_inames, m_Xmm, m_Ymm);
    if (!reader.read(&file)) {
        setErrorString( tr("Parse error in file %1:\n\n%2")
                             .arg(m_inputFile)
                             .arg(reader.errorString()));
        return ResultCode::Error;
    }

    m_nn.setSigma(leaky_relu);
    m_nn.setSigmaPrime(leaky_relu_prime);

    // open input volumes
    m_inputVolumeReaders.clear();

    for( int i=0; i<m_inames.size(); i++){
        auto name=m_inames[i];
        auto r = project()->openVolumeReader(name);
        if( !r ){
            setErrorString( tr("Open volume \"%1\" failed!").arg(name));
            return ResultCode::Error;
        }
        r->setParent(this);

        if( i > 0 ){
            if( r->bounds()!=m_inputVolumeReaders[0]->bounds()){
                setErrorString( tr("Dimensions of volume \"%1\" do not match first volume").arg(name));
                return ResultCode::Error;
            }
        }

        m_inputVolumeReaders.push_back(r);
    }

    auto bounds=m_inputVolumeReaders.front()->bounds();

    // open output volume
    auto writer=project()->openVolumeWriter(m_outputName, bounds, Domain::Other, VolumeType::Other);    // use layer adjusted sampling if required
    if( !writer ){
        setErrorString("Open volume writer failed!");
        return ResultCode::Error;
    }
    writer->setParent(this);

    // do actual nn processing
    emit started(bounds.ni());
    emit currentTask("Computing Samples...");
    qApp->processEvents();

    // process chunks
    for( int il1=bounds.i1(); il1<=bounds.i2(); il1+=CHUNK_ILINES){

        auto il2=std::min(il1+CHUNK_ILINES, bounds.i2());

        std::vector<std::shared_ptr<Volume>> subvols;
        for( int vi=0; vi<m_inputVolumeReaders.size(); vi++){
            auto subvol=m_inputVolumeReaders[vi]->readIl(il1,il2);
            if( !subvol){
                setErrorString(tr("Reading chunk(%1-%2) from volume #%3 failed!").arg(
                                   QString::number(il1), QString::number(il2), QString::number(vi+1)));
                writer->removeFile();
                return ResultCode::Error;
            }
            subvols.push_back(subvol);
        }

        auto sbounds=subvols.front()->bounds();
        auto svolume=std::make_shared<Volume>(sbounds);
        if( !svolume){
            setErrorString("Creating subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        // process chunk

        for(int vi=0; vi<subvols.size(); vi++){
            norm(subvols[vi]->begin(), subvols[vi]->end(), m_Xmm[vi], subvols[vi]->NULL_VALUE);
        }

        Matrix<double> x(1, subvols.size());
        Matrix<double> y( 1, 1);
        for( size_t i=0; i<svolume->size(); i++){

            bool ok=true;
            for( size_t j=0; j<subvols.size(); j++){
                auto v=(*(subvols[j]))[i];
                if( v==subvols[j]->NULL_VALUE ){
                    ok=false;
                    break;
                }
                x(0,j)=v;
            }

            y = m_nn.feed_forward(x);
            //std::cout<<x<<" -> "<<y<<std::endl;
            (*svolume)[i]=y(0,0);
            //emit progress(it);
            //qApp->processEvents();
        }

        unnorm( svolume->begin(), svolume->end(), m_Ymm, svolume->NULL_VALUE);

        if(!writer->write(*svolume)){
            setErrorString("Writing subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        for(int vi=0; vi<subvols.size(); vi++){
            subvols[vi].reset();
        }
        svolume.reset();

        emit progress(il2-bounds.i1());
        qApp->processEvents();
    }

    writer->flush();
    writer->close();



    emit finished();
    qApp->processEvents();

    if( !project()->addVolume( m_outputName, params() ) ) {
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


/*
ProjectProcess::ResultCode NNVolumeClassificationProcess::processWell(QString well){

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

   for( size_t j=0; j<inputLogs.size(); j++){
       norm( inputLogs[j]->begin(), inputLogs[j]->end(), m_Xmm[j], inputLogs[j]->NULL_VALUE);
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

       y = m_nn.feed_forward(x);
       //std::cout<<x<<" -> "<<y<<std::endl;
       (*outputLog)[i]=y(0,0);
       //emit progress(it);
       //qApp->processEvents();
   }

   unnorm( outputLog->begin(), outputLog->end(), m_Ymm, outputLog->NULL_VALUE);

   //emit currentTask("Saving output...");
   //emit started(1);
   //emit progress(0);
   //qApp->processEvents();


   if( !project()->addLog(well, m_outputName, *outputLog)){
       setErrorString("Adding result log to project failed!");
       return ResultCode::Error;
   }

    return ResultCode::Ok;
}
*/