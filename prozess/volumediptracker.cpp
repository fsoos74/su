#include "volumediptracker.h"

#include<iostream>

VolumeDipTracker::VolumeDipTracker( std::shared_ptr<Volume> ildip, std::shared_ptr<Volume> xldip ):
    m_ildip(ildip), m_xldip(xldip){
    if( !m_ildip || !m_xldip  || ildip->bounds()!=xldip->bounds()) throw std::runtime_error("Invalid dip volumes!");
}

double VolumeDipTracker::track( int i, int j, double t, int ii, int jj)const{

    #define TEST

    #ifdef TEST
    bool dump=(i==50) && (j==50 );
    if( dump ){
        std::cout<<"il="<<i<<" xl="<<j<<" t="<<t<<" ii="<<ii<<" jj="<<jj<<std::endl<<std::flush;
    }
    #endif


    auto di = m_ildip->value( i, j, t); //m_ildip->valueAt( i, j, t); valueAt uses sample not time!!!!
    if( di==m_ildip->NULL_VALUE) return t;

    auto dj = m_xldip->value( i, j, t);
    if( dj==m_xldip->NULL_VALUE) return t;

    auto delta_i=ii - i;
    auto delta_j=jj - j;

    double tt = t +  delta_i*di*m_ildip->dt() + delta_j*dj*m_xldip->dt();        // dip volumes store sample per line

    #ifdef TEST
    if( dump ){
        std::cout<<"di="<<di<<" dj="<<dj<<" tt="<<tt<<std::endl<<std::flush;
    }
    #endif

    return tt;
}
