#include "volumediptracker.h"

#include<iostream>

VolumeDipTracker::VolumeDipTracker( std::shared_ptr<Volume> ildip, std::shared_ptr<Volume> xldip ):
    m_ildip(ildip), m_xldip(xldip){
    if( !m_ildip || !m_xldip  || ildip->bounds()!=xldip->bounds()) throw std::runtime_error("Invalid dip volumes!");
}

double VolumeDipTracker::track( int i, int j, double t, int ii, int jj)const{

    auto dims = m_ildip->value( i, j, t);
    if( dims==m_ildip->NULL_VALUE) return t;

    auto djms = m_xldip->value( i, j, t);
    if( djms==m_xldip->NULL_VALUE) return t;

    auto delta_i=ii - i;
    auto delta_j=jj - j;

    double tt = t +  0.001*delta_i*dims + 0.001*delta_j*djms;        // msec -> sec

    return tt;
}
