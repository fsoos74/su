#include "volumeiotestprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>
#include <QApplication>
#include<iostream>
#include<cstring>


VolumeIOTestProcess::VolumeIOTestProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Volume IO Test"), project, parent){

}

ProjectProcess::ResultCode VolumeIOTestProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_outputName=getParam(parameters, "output-volume");
        m_inputName=getParam(parameters, "input-volume");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

/*
// only test reader
ProjectProcess::ResultCode VolumeIOTestProcess::run(){

    auto v=project()->loadVolume(m_inputName);
    if(!v){
        setErrorString("Loadingvolume failed!");
        return ResultCode::Error;
    }
    auto bounds=v->bounds();

    auto writer=project()->openVolumeWriter(m_outputName, bounds, v->domain(), v->type());

    if( !writer ){
        setErrorString("Open volume writer failed!");
        return ResultCode::Error;
    }

    const int CHUNK_ILINES=10;

    emit started(bounds.ni());
    qApp->processEvents();

    for( int il1=bounds.i1(); il1<bounds.i2(); il1+=CHUNK_ILINES){

        auto il2=std::min(il1+CHUNK_ILINES, bounds.i2());
        auto sbounds=Grid3DBounds(il1,il2, bounds.j1(),bounds.j2(), bounds.nt(), bounds.ft(), bounds.dt());
        auto svolume=std::make_shared<Volume>(sbounds);
        if( !svolume){
            setErrorString("Creating subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }
        for( int i=sbounds.i1(); i<sbounds.i2(); i++){
            for( int j=sbounds.j1(); j<sbounds.j2(); j++){
                for( int k=0; k<sbounds.nt(); k++){
                    (*svolume)(i,j,k)=(*v)(i,j,k);
                }
            }
        }

        if(!writer->write(*svolume)){
            setErrorString("Writing subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        emit progress(il2-bounds.i1());
        qApp->processEvents();
    }

    writer->flush();
    writer->close();

    if( !project()->addVolume( m_outputName, params() ) ){
        writer->removeFile();
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
*/

/* test reader and writer
ProjectProcess::ResultCode VolumeIOTestProcess::run(){

    auto reader=project()->openVolumeReader(m_inputName);

    if( !reader || !reader->good()){
        setErrorString("Open volume reader failed!");
        return ResultCode::Error;
    }

    auto bounds=reader->bounds();

    auto writer=project()->openVolumeWriter(m_outputName, bounds, reader->domain(), reader->type());

    if( !writer ){
        setErrorString("Open volume writer failed!");
        return ResultCode::Error;
    }

    const int CHUNK_ILINES=10;

    emit started(bounds.ni());
    qApp->processEvents();

    for( int il1=bounds.i1(); il1<=bounds.i2(); il1+=CHUNK_ILINES){

        auto il2=std::min(il1+CHUNK_ILINES, bounds.i2());
        auto subvolume=reader->readIl(il1, il2);
        if( !subvolume){
            setErrorString("Reading subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        if(!writer->write(*subvolume)){
            setErrorString("Writing subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        emit progress(il2-bounds.i1());
        qApp->processEvents();
    }

    reader->close();
    writer->close();

    if( !project()->addVolume( m_outputName, params() ) ){
        writer->removeFile();
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
*/

// test reader and writer for time slices
ProjectProcess::ResultCode VolumeIOTestProcess::run(){

    auto reader=project()->openVolumeReader(m_inputName);

    if( !reader || !reader->good()){
        setErrorString("Open volume reader failed!");
        return ResultCode::Error;
    }

    auto bounds=reader->bounds();

    auto writer=project()->openVolumeWriter(m_outputName, bounds, reader->domain(), reader->type());

    if( !writer ){
        setErrorString("Open volume writer failed!");
        return ResultCode::Error;
    }

    if( !writer->good()){
        setErrorString(writer->lastError());
        return ResultCode::Error;
    }

    const int CHUNK_SAMPLES=10;

    emit started(bounds.nt());
    qApp->processEvents();

    for( int k0=0; k0<bounds.nt(); k0+=CHUNK_SAMPLES){

        auto subvolume=reader->readK(k0, k0+CHUNK_SAMPLES-1);
        if( !subvolume){
            setErrorString(QString("Reading subvolume failed: %1").arg(reader->lastError()));
            writer->removeFile();
            return ResultCode::Error;
        }

        if(!writer->writeK(*subvolume)){
            setErrorString(QString("Writing subvolume failed: %1").arg(writer->lastError()));
            writer->removeFile();
            return ResultCode::Error;
        }

        emit progress(k0);
        qApp->processEvents();
    }

    reader->close();
    writer->close();

    if( !project()->addVolume( m_outputName, params() ) ){
        writer->removeFile();
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
