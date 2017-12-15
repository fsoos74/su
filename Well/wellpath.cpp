#include <wellpath.h>
#include<iostream>


WellPath::WellPath() {

}

WellPath::WellPath( QVector<QVector3D> points) : m_xyz(points), m_md(points.size()){
    computeMD();
}

WellPath WellPath::fromLog(const Log &log, qreal x, qreal y){

    QVector<QVector3D> points;
    points.reserve(log.nz());
    for( int i=0; i<log.nz(); i++){
        auto z=-log.index2z(i);             // z not depth, index2z means index to depth
        points.push_back(QVector3D(x,y,z));
    }
    return WellPath(points);
}


QPointF WellPath::locationAtZ( qreal z)const{ // z-axis points up, wellpath assumed z-sorted (descending)

    int idx=findLEByZ(z);
    if( idx<0 ) return QPointF();
    if( idx+1 >= m_xyz.size() )  return QPointF( m_xyz.back().x(), m_xyz.back().y() );

    // interpolate coords
    qreal ztop=m_xyz[idx].z();
    qreal zbottom=m_xyz[idx+1].z();
    qreal wtop=(z-zbottom)/(ztop-zbottom);
    qreal wbottom=1.-wtop;
    qreal x=wtop*m_xyz[idx].x() + wbottom*m_xyz[idx+1].x();
    qreal y=wtop*m_xyz[idx].y() + wbottom*m_xyz[idx+1].y();
    return QPointF(x,y);
}

QPointF WellPath::locationAtTVD( qreal tvd)const{
    auto z = zAtTVD(tvd);
    return locationAtZ(z);
}

QPointF WellPath::locationAtMD( qreal md)const{
    auto z = zAtMD(md);
    return locationAtZ(z);
}

qreal WellPath::tvdAtMD( qreal md)const{
    auto z = zAtMD(md);
    return tvdAtZ(z);
}

qreal WellPath::tvdAtZ( qreal z)const{
    return -z; // + m_xyz.front().z()
}

qreal WellPath::mdAtTVD( qreal tvd)const{
   auto z = zAtTVD(tvd);
   return mdAtZ(z);
}

qreal WellPath::mdAtZ( qreal z)const{
    int idx=findLEByZ(z);
    if( idx<0 ) return m_md.front();
    if( idx+1 >= m_md.size() )  return m_md.back();

    // interpolate
    qreal mdtop=m_md[idx];
    qreal mdbottom=m_md[idx+1];
    qreal ztop=m_xyz[idx].z();
    qreal zbottom=m_xyz[idx+1].z();
    qreal wtop=(z-zbottom)/(ztop-zbottom);
    qreal wbottom=1.-wtop;
    qreal md=wtop*mdtop + wbottom*mdbottom;

    if( false /*int(z)==-5965*/){
        std::cout<<"z="<<z<<" idx="<<idx<<" ztop="<<ztop<<" zbottom="<<zbottom<<" mdtop="<<mdtop<<" mdbottom="<<mdbottom;
        std::cout<<" wtop="<<wtop<<" wbottom="<<wbottom<<" md="<<md<<std::endl<<std::flush;
    }


    return md;
}

qreal WellPath::zAtTVD( qreal tvd) const{
    return -tvd;    // NEED to consider reference point/top
}

qreal WellPath::zAtMD( qreal md) const{
    int idx=findLEByMD(md);
    if( idx<0 ) return m_xyz.front().z();
    if( idx+1 >= m_xyz.size() )  return m_xyz.back().z();

    // interpolate
    qreal mdtop=m_md[idx];
    qreal mdbottom=m_md[idx+1];
    qreal ztop=m_xyz[idx].z();
    qreal zbottom=m_xyz[idx+1].z();
    qreal wtop=(md-mdbottom)/(mdtop-mdbottom);
    qreal wbottom=1.-wtop;
    qreal z=wtop*ztop + wbottom*zbottom;

    if( false){
        std::cout<<"md="<<md<<" idx="<<idx<<" mdtop="<<mdtop<<" mdbottom="<<mdbottom<<" ztop="<<ztop<<" zbottom="<<zbottom;
        std::cout<<" wtop="<<wtop<<" wbottom="<<wbottom<<" z="<<z<<std::endl<<std::flush;
    }

    return z;
}

void WellPath::computeMD(){

    if( m_xyz.empty()) return;

    m_md[0]=0;
    for( int i = 1; i<m_xyz.size(); i++){
        m_md[i] = m_md[i-1] + m_xyz[i].distanceToPoint(m_xyz[i-1]);
    }
}


// find index of datapoint before z or at z, return -1 if not found
int WellPath::findLEByZ( qreal z)const{  // z-axis points up, wellpath assumed z-sorted (descending)

    if( m_xyz.empty()) return -1;

    int top=0;
    int bottom=m_xyz.size()-1;
    if( z>=m_xyz.front().z()) return top;
    if( z<=m_xyz.back().z()) return bottom;

    //find index<=depth
    while( top+1<bottom ){

        int mid=(top+bottom)/2;

        if( m_xyz[mid].z()<z){
            bottom=mid;
            continue;
        }

        if( m_xyz[mid].z()>=z){
            top=mid;
            continue;
        }

    }

    return top;
}


// find index of datapoint before md or at md, return -1 if not found
int WellPath::findLEByMD( qreal md)const{  // md assumed sorted (ascending)

    if( m_md.empty()) return -1;

    int top=0;
    int bottom=m_md.size()-1;
    if( md<=m_md.front()) return top;
    if( md>=m_md.back()) return bottom;

    //find index<=md
    while( top+1<bottom ){

        int mid=(top+bottom)/2;

        if( m_md[mid]>md){
            bottom=mid;
            continue;
        }

        if( m_md[mid]<md){
            top=mid;
            continue;
        }

        break;
    }

    return top;
}
