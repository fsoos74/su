#include "volumereader2.h"

#include<cstring>
#include<QApplication>  // qApp
#include<cmath>
#include<iostream>


VolumeReader2::VolumeReader2(const QString& fname, QObject* parent ):QObject(parent),m_file(fname){

}

bool VolumeReader2::open(){

    m_file.open(QFile::ReadOnly);

    if( m_file.error()!=QFile::FileError::NoError){
        setError(m_file.errorString());
        return false;
    }

    if( !readMagic(m_file)) return false;

    if(!readBounds(m_file, m_bounds )){
        setError("Reading volume dimensions failed!");
        return false;
    }

    if(m_version==2){
        if(!readDomainAndType(m_file, m_domain, m_type)){
            setError("Reading dmain and type failed!");
            return false;
        }
    }

    m_dataStart=m_file.pos();

    return true;
}

void VolumeReader2::close(){
    m_file.close();
}

std::shared_ptr<Volume> VolumeReader2::read(){
    return readData(m_bounds);
}

std::shared_ptr<Volume> VolumeReader2::readIl(int i1, int i2){

    if( i1>i2 ) return std::shared_ptr<Volume>();
    if( i1<m_bounds.i1() || i2>m_bounds.i2()){
        setError("Requested inline range out of bounds");
        return std::shared_ptr<Volume>();
    }

    Grid3DBounds bounds(i1, i2, m_bounds.j1(), m_bounds.j2(), m_bounds.nt(), m_bounds.ft(), m_bounds.dt());

    return readData(bounds);
}

std::shared_ptr<Volume> VolumeReader2::readXl(int j1, int j2){
    return std::shared_ptr<Volume>();
}

std::shared_ptr<Volume> VolumeReader2::readK(int k1, int k2){

    std::shared_ptr<Volume> volume;

    if( k1>k2 ) return volume;
    if( k1<0 || k2>=m_bounds.nt()){
        setError("Requested sample range out of bounds");
        return volume;
    }

    auto nk=k2-k1+1;
    Grid3DBounds bounds(m_bounds.i1(), m_bounds.i2(), m_bounds.j1(), m_bounds.j2(),
                        nk, m_bounds.ft()+k1*m_bounds.dt(), m_bounds.dt());

    try{
        volume=std::make_shared<Volume>(bounds);
    }
    catch(std::exception& ex){
        setError(QString("Allocating volume failed: ") + QString(ex.what() ));
        return std::shared_ptr<Volume>();
    }

    readDataK((char*)volume->data(), k1, nk);

    return volume;
}



std::shared_ptr<Volume> VolumeReader2::readData(const Grid3DBounds& bounds){

    std::shared_ptr<Volume> volume;

    try{
        volume=std::make_shared<Volume>(bounds);
    }
    catch(std::exception& ex){
        setError(QString("Allocating volume failed: ") + QString(ex.what() ));
        return std::shared_ptr<Volume>();
    }

    volume->setDomain(m_domain);
    volume->setType(m_type);
    if( !readDataIl((char*) volume->data(), volume->bounds().i1(), volume->bounds().ni())) volume.reset();

    return volume;
}


bool VolumeReader2::readMagic(QFile& file){

    const size_t MAGIC_LEN=19;
    char MAGIC[]="Binary Volume File";
    char magic[MAGIC_LEN];

    file.read(magic, MAGIC_LEN);
    if( file.error()!=QFile::FileError::NoError){
        setError("Reading file id failed!");
        return false;
    }

    if(strncmp(magic, MAGIC, MAGIC_LEN!=0)){
        setError("Wrong file id!");
        return false;
    }

    const size_t VERSION2_MAGIC_LEN=9;
    char VERSION2_MAGIC[]="version2";
    char version2magic[VERSION2_MAGIC_LEN];

    file.read(version2magic, VERSION2_MAGIC_LEN);
    if( file.error()!=QFile::FileError::NoError){
        setError("Reading file id2 failed!");
        return false;
    }
    m_version=1;

    if(strncmp(version2magic, VERSION2_MAGIC, VERSION2_MAGIC_LEN)==0){
        m_version=2;
    }
    else{
        file.seek(file.pos()-VERSION2_MAGIC_LEN);
    }

    return (file.error()==QFile::FileError::NoError);
}


bool VolumeReader2::readBounds(QFile& file, Grid3DBounds& bounds){

    qint64 il1;
    file.read((char*)&il1, sizeof(il1));
    qint64 il2;
    file.read((char*)&il2, sizeof(il2));

    qint64 xl1;
    file.read((char*)&xl1, sizeof(xl1));
    qint64 xl2;
    file.read((char*)&xl2, sizeof(xl2));

    qreal ft;
    file.read((char*)&ft, sizeof(ft));
    qreal dt;
    file.read((char*)&dt, sizeof(dt));
    qint64 sampleCount;
    file.read((char*)&sampleCount, sizeof(sampleCount));

    if(file.error()!=QFile::FileError::NoError) return false;

    bounds=Grid3DBounds(il1,il2, xl1,xl2, sampleCount, ft, dt);

    return true;
}


bool VolumeReader2::readDomainAndType(QFile& file, Domain &domain, VolumeType &type){

    qint16 dom;
    file.read((char*)&dom, sizeof(dom));

    qint16 typ;
    file.read((char*)&typ, sizeof(typ));

    if(file.error()!=QFile::FileError::NoError) return false;

    domain=static_cast<Domain>(dom);
    type=static_cast<VolumeType>(typ);

    return true;
}

/*
bool VolumeReader::readData(std::ifstream& istr, float* p, size_t count ){

    istr.read( (char*)p, count*sizeof(float));
    if(!istr.good()){
        setError("Reading volume data failed!");
        return false;
    }

    return true;
}
*/

// reads whole inlines from file
// first inline=il0, number of inlines=nil
// p must point to a buffer that has nil * (number of xlines) * (number of samples) * sizeof(float) memory
bool VolumeReader2::readDataIl(char* p, int il0, int nil){

    const qint64 ilsize=m_bounds.nj()*m_bounds.nt()*sizeof(float);
    qint64 pos0 = m_dataStart + (il0-m_bounds.i1())*ilsize;
    m_file.seek(pos0);

    for( int i=0; i<nil; i++){
        if( m_file.read(p, ilsize)!=ilsize ) return false;
        p+=ilsize;
        int perc=static_cast<int>(100*qreal(i)/nil);
        emit percentDone(perc);
        qApp->processEvents();
    }

    return true;
}


// reads whole time/depth slices
// first slice = k0, nk = number of slices
// p must point to a buffer that has (number of ilines ) * (number of xlines) * nk * sizeof(float) memory
bool VolumeReader2::readDataK(char* p, int k0, int nk){

    // iterate over cdps
    for( int i = 0; i<m_bounds.ni()*m_bounds.nj(); i++){

        qint64 pos0=m_dataStart + (i*m_bounds.nt() + k0)*sizeof(float);
        auto pd= p + i*nk*sizeof(float);
        unsigned n=nk*sizeof(float);
        m_file.seek(pos0);
        if( m_file.error()!=QFile::NoError) return false;
        if( m_file.read(pd, n)!=n) return false;
    }

    return true;
}


void VolumeReader2::setError(const QString& msg){
    m_file.close();
    m_errorString=msg;
}
