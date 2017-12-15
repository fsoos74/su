#include "seismicdatasetinfo.h"

#include <stdexcept>
#include<QDir>

void SeismicDatasetInfo::makeRelative( const QString& dir){

    // make abolute paths relative to dir
    if( QDir(infoPath()).isAbsolute() ){
        QString s=QDir(dir).relativeFilePath(infoPath());
        setInfoPath(s);
    }

    if( QDir(indexPath()).isAbsolute()){
        QString s=QDir(dir).relativeFilePath(indexPath() );
        setIndexPath(s);
    }

    if( QDir(path()).isAbsolute()){
        QString s=QDir(dir).relativeFilePath(path());
        setPath(s);
    }
}


void SeismicDatasetInfo::makeAbsolute( const QString& dir){

    // make abolute paths relative to dir
    if( QDir(infoPath()).isRelative() ){
        QString s=QDir(dir).absoluteFilePath(infoPath());
        setInfoPath(s);
    }

    if( QDir(indexPath()).isRelative()){
        QString s=QDir(dir).absoluteFilePath(indexPath() );
        setIndexPath(s);
    }

    if( QDir(path()).isRelative()){
        QString s=QDir(dir).absoluteFilePath(path());
        setPath(s);
    }

}


void SeismicDatasetInfo::setName(const QString& name){
    m_name=name;
}

void SeismicDatasetInfo::setPath(const QString& path){
    m_path=path;
}

void SeismicDatasetInfo::setInfoPath(const QString& path){
    m_infoPath=path;
}

void SeismicDatasetInfo::setIndexPath(const QString& path){
    m_indexPath=path;
}

void SeismicDatasetInfo::setDomain( SeismicDatasetInfo::Domain domain){
    m_domain=domain;
}

void SeismicDatasetInfo::setMode( SeismicDatasetInfo::Mode mode){
    m_mode=mode;
}

void SeismicDatasetInfo::setDT(double dt){
    m_dt=dt;
}

void SeismicDatasetInfo::setFT(double ft){
    m_ft=ft;
}

void SeismicDatasetInfo::setNT(size_t nt){
    m_nt=nt;
}

const QString TimeStr="Time";
const QString DepthStr="Depth";

QString datasetDomainToString( SeismicDatasetInfo::Domain domain){

    switch(domain){
    case SeismicDatasetInfo::Domain::Time: return TimeStr; break;
    case SeismicDatasetInfo::Domain::Depth: return DepthStr; break;
    default:
        throw std::runtime_error("Unhandled case in datasetDomainToString");
        break;
    }
}

SeismicDatasetInfo::Domain stringToDatasetDomain(const QString& str){

    if( str==TimeStr){
        return SeismicDatasetInfo::Domain::Time;
    }
    else if( str==DepthStr){
        return SeismicDatasetInfo::Domain::Depth;
    }
    else{
        throw std::runtime_error("Unexpected string in stringToDatasetDomain");
    }
}

const QString PoststackStr="Poststack";
const QString PrestackStr="Prestack";

QString datasetModeToString( SeismicDatasetInfo::Mode mode){

    switch(mode){
    case SeismicDatasetInfo::Mode::Poststack: return PoststackStr; break;
    case SeismicDatasetInfo::Mode::Prestack: return PrestackStr; break;
    default:
        throw std::runtime_error("Unhandled case in datasetModeToString");
        break;
    }
}

SeismicDatasetInfo::Mode stringToDatasetMode(const QString& str){

    if( str==PrestackStr ){
        return SeismicDatasetInfo::Mode::Prestack;
    }
    else if( str==PoststackStr){
        return SeismicDatasetInfo::Mode::Poststack;
    }
    else{
        throw std::runtime_error("Unexpected string in stringToDatasetMode");
    }
}

