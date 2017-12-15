#include "volumediptracker.h"

VolumeDipTracker::VolumeDipTracker( std::shared_ptr<Volume> ildip, std::shared_ptr<Volume> xldip ):
    m_ildip(ildip), m_xldip(xldip){
    if( !m_ildip || !m_xldip  || ildip->bounds()!=xldip->bounds()) throw std::runtime_error("Invalid dip volumes!");
}

double VolumeDipTracker::track( int i, int j, double t, int ii, int jj)const{

    auto di = m_ildip->valueAt( i, j, t);
    if( di==m_ildip->NULL_VALUE) return t;

    auto dj = m_xldip->valueAt( i, j, t);
    if( dj==m_xldip->NULL_VALUE) return t;

    auto delta_i=ii - i;
    auto delta_j=jj - j;

    return t + delta_i*di + delta_j*dj;
}
