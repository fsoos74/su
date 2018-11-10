#include "volumesprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include<iostream>
#include<QApplication>


VolumesProcess::VolumesProcess( QString name, AVOProject* project, QObject* parent) :
    ProjectProcess( name, project, parent){
}

Grid3DBounds VolumesProcess::inputBounds(int i)const{
#ifdef IO_VOLUMES
    Q_ASSERT( i>=0 && i<m_inputReaders.size());
    return m_inputReaders[i]->bounds();
#else
    Q_ASSERT( i>=0 || i<m_inputVolumes.size());
    return m_inputVolumes[i]->bounds();
#endif
}

 Domain VolumesProcess::inputDomain(int i)const{
#ifdef IO_VOLUMES
    Q_ASSERT( i>=0 && i<m_inputReaders.size());
    return m_inputReaders[i]->domain();
#else
    Q_ASSERT( i>=0 || i<m_inputVolumes.size());
    return m_inputVolumes[i]->domain();
#endif
}

VolumeType VolumesProcess::inputType(int i)const{
#ifdef IO_VOLUMES
    Q_ASSERT( i>=0 && i<m_inputReaders.size());
    return m_inputReaders[i]->type();
#else
    Q_ASSERT( i>=0 || i<m_inputVolumes.size());
    return m_inputVolumes[i]->type();
#endif
}

ProjectProcess::ResultCode VolumesProcess::addInputVolume(QString name){

#ifdef IO_VOLUMES
    auto reader=project()->openVolumeReader(name);
    if(!reader){
        setErrorString(QString("Open input volume \"%1\" failed!").arg(name));
        return ResultCode::Error;
    }
    reader->setParent(this);        // take ownership
    m_inputReaders.push_back(reader);
#else
    auto volume=project()->loadVolume( name);
    if( !volume ){
        setErrorString(QString("Loading volume \"%1\" failed!").arg(name));
        return ResultCode::Error;
    }
    m_inputVolumes.push_back(volume);
#endif
    m_inputVolumeNames.push_back(name);
    return ResultCode::Ok;
}


ProjectProcess::ResultCode VolumesProcess::addOutputVolume(QString name, Grid3DBounds bounds, Domain domain, VolumeType type){

#ifdef IO_VOLUMES
    auto writer=project()->openVolumeWriter(name, bounds, domain, type );
    if(!writer){
        setErrorString(QString("Open volume writer \"%1\" failed!").arg(name));
        return ResultCode::Error;
    }
    writer->setParent(this);        // take ownership, no need to delete
    m_outputWriters.push_back(writer);
#else
    auto volume=std::make_shared<Volume>(bounds);
    if( !volume){
        setErrorString(QString("Allocating volume \"%1\" failed!").arg(name));
        return ResultCode::Error;
    }
    volume->setDomain(domain);
    volume->setType(type);
    m_outputVolumes.push_back(volume);
#endif
    m_outputVolumeNames.push_back(name);
    return ResultCode::Ok;
}


ProjectProcess::ResultCode VolumesProcess::run(){

    QVector<std::shared_ptr<Volume> > inputVolumes;
    QVector<std::shared_ptr<Volume> > outputVolumes;
#ifdef IO_VOLUMES
    for( int i=0; i<m_inputReaders.size(); i++){
        inputVolumes.push_back(std::shared_ptr<Volume>());
    }
    for( int i=0; i<m_outputWriters.size(); i++){
        outputVolumes.push_back(std::shared_ptr<Volume>());
    }
#else
    for( int i=0; i<m_inputVolumes.size(); i++){
        inputVolumes.push_back(m_inputVolumes[i]);
    }
    for( int i=0; i<m_outputVolumes.size(); i++){
        outputVolumes.push_back(m_outputVolumes[i]);
    }
#endif

    emit currentTask("Computing output volumes");
    emit started(m_bounds.ni());
    qApp->processEvents();

    for( int il=m_bounds.i1(); il<=m_bounds.i2(); il++){
#ifdef IO_VOLUMES
        // save chunk if completed
        // all output (sub)volumes are allocated simultaneously and have the same geometry
        if( (!outputVolumes.isEmpty()) &&
                ( (outputVolumes.front() && (il>outputVolumes.front()->bounds().i2() ) ) ) ){

           for( int i=0; i<m_outputWriters.size(); i++){
                if( (outputVolumes[i]) && !m_outputWriters[i]->write(*outputVolumes[i])){
                    setErrorString(QString("Writing subvolume \"%1\" failed!").arg(m_outputVolumeNames[i]));
                    return ResultCode::Error;
                }
                outputVolumes[i].reset();

            }
        }

        // last inline of next chunk
        int il2=std::min(il+CHUNK_SIZE, m_bounds.i2());

        // allocate new output subvolumes if required
        if( (!outputVolumes.isEmpty() ) && ( !outputVolumes.front() ) ){
            Grid3DBounds sbounds(il,il2, m_bounds.j1(),m_bounds.j2(), m_bounds.nt(), m_bounds.ft(),m_bounds.dt());
            for( int i=0; i<outputVolumes.size(); i++){
                auto volume=std::make_shared<Volume>(sbounds);
                if(!volume){
                    setErrorString(QString("Allocating subvolume \"%1\" failed!").arg(m_outputVolumeNames[i]));
                    return ResultCode::Error;
                }
                outputVolumes[i]=volume;
            }
        }

        // load next chunk if required
        if( (!inputVolumes.empty()) && ( (!inputVolumes.front()) || (il>inputVolumes.front()->bounds().i2()) ) ){
        //if( (!inputVolumes.empty()) && (il>inputVolumes.front()->bounds().i2() ) ){
            for(int i=0; i<inputVolumes.size(); i++){
                auto volume=m_inputReaders[i]->readIl(il,il2);
                if(!volume){
                    setErrorString(QString("Reading input subvolume \"%1\" failed!").arg(m_inputVolumeNames[i]));
                    return ResultCode::Error;
                }
                inputVolumes[i]=volume;
            }
        }
 #endif

        auto res=processInline(outputVolumes, inputVolumes, il);
        if( res!=ResultCode::Ok){
            return res;
        }

        emit progress(il-m_bounds.i1() );
        qApp->processEvents();

        if(isCanceled()) return ResultCode::Canceled;
    }

 #ifdef IO_VOLUMES
    // save last chunk
    for( int i=0; i<m_outputWriters.size(); i++){
        if(!m_outputWriters[i]->write(*outputVolumes[i])){
            setErrorString(QString("Writing subvolume \"%1\" failed!").arg(m_outputVolumeNames[i]));
            return ResultCode::Error;
        }
    }

    // register volumes
    for(int i=0; i<m_outputVolumeNames.size(); i++){
        if( !project()->addVolume( m_outputVolumeNames[i], params() ) ) {
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputVolumeNames[i]) );
            return ResultCode::Error;
        }
    }

 #else

    emit currentTask("Saving result volumes");
    emit started(m_outputVolumes.size());
    emit progress(0);
    qApp->processEvents();

    for(int i=0; i<m_outputVolumes.size(); i++){
        if( !project()->addVolume( m_outputVolumeNames[i], m_outputVolumes[i], params() ) ) {
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputVolumeNames[i]) );
            return ResultCode::Error;
        }
        emit progress(i+1);
        qApp->processEvents();
    }

#endif

    auto result=finalize();

    emit finished();
    qApp->processEvents();

    return result;
}
