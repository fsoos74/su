#include "volumewriter2.h"

VolumeWriter2::VolumeWriter2(const QString& fname):m_file(fname){

}


bool VolumeWriter2::open(const Grid3DBounds &bounds, const Domain &domain, const VolumeType &type){

    m_file.open(QFile::WriteOnly);
    if( m_file.error()!=QFile::FileError::NoError){
        setError("Open file failed!");
        return false;
    }

    if( !writeMagic(m_file)){
        setError("Writing magic failed!");
        return false;
    }

    if( !writeBounds(m_file, bounds)){
        setError("Writing bounds failed!");
        return false;
    }

    if( !writeDomainAndType(m_file, domain, type)){
        setError("Writing domain and type failed!");
        return false;
    }

    qint64 filesize = m_file.pos() + bounds.ni()*bounds.nj()*bounds.nt()*sizeof(float);
    m_file.resize(filesize);
    if( m_file.error()!=QFile::FileError::NoError) return false;

    m_bounds=bounds;
    m_dataStart=m_file.pos();

    return true;
}

void VolumeWriter2::flush(){
    m_file.flush();
}

void VolumeWriter2::close(){
    m_file.close();
}

bool VolumeWriter2::write(const Volume&  volume){

    if(m_file.error()!=QFile::FileError::NoError) return false;

    if( (volume.bounds().nj()!=m_bounds.nj() ) ||
            (volume.bounds().nt()!=m_bounds.nt() ) ){
        setError("Volume has invalid bounds!");
        return false;
    }

    return writeData((const char*) volume.data(), volume.bounds().i1(), volume.bounds().ni());
}

void VolumeWriter2::removeFile(){
    m_file.remove();
}

bool VolumeWriter2::writeMagic(QFile& file){

    const size_t MAGIC_LEN=19;
    char MAGIC[]="Binary Volume File";

    file.write(MAGIC, MAGIC_LEN);

    const size_t VERSION2_MAGIC_LEN=9;
    char VERSION2_MAGIC[]="version2";

    file.write(VERSION2_MAGIC, VERSION2_MAGIC_LEN );

    return (file.error()==QFile::FileError::NoError);
}

bool VolumeWriter2::writeBounds(QFile& file, const Grid3DBounds& bounds){

    qint64 il1=bounds.i1();
    file.write((char*)&il1,sizeof(il1));
    qint64 il2=bounds.i2();
    file.write((char*)&il2,sizeof(il2));

    qint64 xl1=bounds.j1();
    file.write((char*)&xl1,sizeof(xl1));
    qint64 xl2=bounds.j2();
    file.write((char*)&xl2,sizeof(xl2));

    qreal ft=bounds.ft();
    file.write((char*)&ft, sizeof(ft));
    qreal dt=bounds.dt();
    file.write((char*)&dt, sizeof(dt));
    qint64 sampleCount=bounds.nt();
    file.write((char*)&sampleCount,sizeof(sampleCount));

    return (file.error()==QFile::FileError::NoError);
}

bool VolumeWriter2::writeDomainAndType(QFile& file, const Domain& domain, const VolumeType& type){

    qint16 idom=static_cast<qint16>(domain);
    file.write((char*)&idom, sizeof(idom));

    qint16 itype=static_cast<qint16>(type);
    file.write((char*)&itype, sizeof(itype));

    return (file.error()==QFile::FileError::NoError);
}

// writes whole inlines from file
// first inline=il0, number of inlines=nil
// p must point to a buffer that has nil * (number of xlines) * sizeof(float) memory
bool VolumeWriter2::writeData(const char* p, int il0, int nil){

    const qint64 ilsize=m_bounds.nj()*m_bounds.nt()*sizeof(float);
    qint64 pos0 = m_dataStart + (il0-m_bounds.i1())*ilsize;
    m_file.seek(pos0);
    if( m_file.error()!=QFile::FileError::NoError) return false;
    if( m_file.write(p, nil*ilsize)!=nil*ilsize ) return false;

    return true;
}

void VolumeWriter2::setError(const QString& msg){
    m_file.close();
    m_errorString=msg;
}

