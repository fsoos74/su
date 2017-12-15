#include "volume.h"

Volume::Volume(const Grid3DBounds& bounds, const Domain& domain, const VolumeType& type, const float& ival ):
    Grid3D<float>(bounds, ival), m_domain(domain), m_type(type)
{

}


void Volume::setDomain(Domain d){
    m_domain=d;
}


void Volume::setType(VolumeType t){
    m_type=t;
}
