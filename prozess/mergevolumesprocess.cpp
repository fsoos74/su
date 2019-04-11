#include "mergevolumesprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

 using namespace std::placeholders;

MergeVolumesProcess::MergeVolumesProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Merge Volumes"), project, parent){
    m_null=Grid3D<float>::NULL_VALUE;
}

ProjectProcess::ResultCode MergeVolumesProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_outputName=getParam(parameters, "output");
        m_volume1Name=getParam(parameters, "volume1");
        m_volume2Name=getParam(parameters, "volume2");
        m_minIline1=getParam(parameters,"min-iline1").toInt();
        m_maxIline1=getParam(parameters,"max-iline1").toInt();
        m_minIline2=getParam(parameters,"min-iline2").toInt();
        m_maxIline2=getParam(parameters,"max-iline2").toInt();
        m_ov=getParam(parameters,"overlap").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode MergeVolumesProcess::run(){

    // open input volume 1
    m_volume1Reader=project()->openVolumeReader(m_volume1Name);
    if(!m_volume1Reader){
        setErrorString("Open input volume 1 failed!");
        return ResultCode::Error;
    }
    m_volume1Reader->setParent(this);

    // open input volume 2
    m_volume2Reader=project()->openVolumeReader(m_volume2Name);
    if(!m_volume2Reader){
        setErrorString("Open input volume 2 failed!");
        return ResultCode::Error;
    }
    m_volume2Reader->setParent(this);

    auto ibounds1=m_volume1Reader->bounds();
    auto ibounds2=m_volume2Reader->bounds();
    if( ibounds1.dt()!=ibounds2.dt() || ibounds1.ft()!=ibounds2.ft() ||
            ibounds1.nt()!=ibounds2.nt()){
        setErrorString("Input volumes have different sampling!");
        return ResultCode::Error;
    }

    auto i1=std::min(ibounds1.i1(),ibounds2.i1());
    auto i2=std::max(ibounds1.i2(),ibounds2.i2());
    auto j1=std::min(ibounds1.j1(),ibounds2.j1());
    auto j2=std::max(ibounds1.j2(),ibounds2.j2());

    m_bounds=Grid3DBounds(i1,i2,j1,j2,ibounds1.nt(),ibounds1.ft(),ibounds1.dt());

    if( initFunction()!=ResultCode::Ok){
        return ResultCode::Error;
    }

    auto writer=project()->openVolumeWriter(m_outputName, m_bounds, Domain::Other, VolumeType::Other);    // use layer adjusted sampling if required
    if( !writer ){
        setErrorString("Open volume writer failed!");
        return ResultCode::Error;
    }

    emit started(m_bounds.ni());
    emit currentTask("Computing attribute...");
    qApp->processEvents();

    // process inlines
    for( int il=m_bounds.i1(); il<=m_bounds.i2(); il++){

        std::shared_ptr<Volume> subVolume1;
        if(il>=ibounds1.i1() && il<=ibounds1.i2() && il>=m_minIline1 && il<=m_maxIline1){
            subVolume1=m_volume1Reader->readIl(il,il);
            if( !subVolume1){
                setErrorString("Reading subvolume 1 failed!");
                writer->removeFile();
                return ResultCode::Error;
            }
        }
        else{
                subVolume1=std::make_shared<Volume>();
        }

        std::shared_ptr<Volume> subVolume2;
        if(il>=ibounds2.i1() && il<=ibounds2.i2() && il>=m_minIline2 && il<=m_maxIline2){
            subVolume2=m_volume2Reader->readIl(il,il);
            if( !subVolume2){
                setErrorString("Reading subvolume 2 failed!");
                writer->removeFile();
                return ResultCode::Error;
            }
        }
        else{
                subVolume2=std::make_shared<Volume>();
        }

        Grid3DBounds sbounds(il, il, m_bounds.j1(), m_bounds.j2(), m_bounds.nt(), m_bounds.ft(), m_bounds.dt());
        auto svolume=std::make_shared<Volume>(sbounds);
        if( !svolume){
            setErrorString("Creating subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        // process inline
        for( int j=m_bounds.j1(); j<=m_bounds.j2(); j++){

            for( int k=0; k<m_bounds.nt(); k++){                              // iterate over output samples
                auto v1=subVolume1->valueAt(il,j,k);
                auto v2=subVolume2->valueAt(il,j,k);
                (*svolume)(il,j,k)=m_func(v1,v2);
            }
        }


        if(!writer->write(*svolume)){
            setErrorString("Writing subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        subVolume1.reset();
        subVolume2.reset();
        svolume.reset();

        emit progress(il-m_bounds.i1());
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

float MergeVolumesProcess::OV1(float v1, float v2 ){
    if(v1!=m_null && std::isfinite(v1)) return v1;
    if(v2!=m_null && std::isfinite(v2)) return v2;
    return m_null; 
}

float MergeVolumesProcess::OV2(float v1, float v2 ){
    if(v2!=m_null && std::isfinite(v2)) return v2;
    if(v1!=m_null && std::isfinite(v1)) return v1;
    return m_null; 
}

float MergeVolumesProcess::OMean(float v1, float v2 ){
    if(v1!=m_null && v2!=m_null) return (v1+v2)/2;
    if(v1!=m_null) return v1;
    if(v2!=m_null) return v2;
    return m_null; 
}

ProjectProcess::ResultCode MergeVolumesProcess::initFunction(){
    switch(m_ov){

    case 0: m_func= std::bind(&MergeVolumesProcess::OV1, this, _1, _2) ; break;
    case 1: m_func= std::bind(&MergeVolumesProcess::OV2, this, _1, _2) ; break;
    case 2: m_func= std::bind(&MergeVolumesProcess::OMean, this, _1, _2) ; break;
    default:
        setErrorString("Invalid overlap type!");
        return ResultCode::Error;
        break;
    }
    return ResultCode::Ok;
}


