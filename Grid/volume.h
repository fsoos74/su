#ifndef VOLUME_H
#define VOLUME_H


#include <grid3d.h>
#include <domain.h>
#include <volumetype.h>


class Volume : public Grid3D<float>
{
public:

    Volume(const Volume& other) = default;

    Volume( const bounds_type& bounds=Grid3DBounds(0,1,0,1,1,0,1),
            const Domain& domain=Domain::Time, const VolumeType& type=VolumeType::Other, const value_type& ival=NULL_VALUE);


    Domain domain()const{
        return m_domain;
    }

    VolumeType type()const{
        return m_type;
    }

    void setDomain(Domain);
    void setType(VolumeType);

private:

    Domain m_domain;
    VolumeType m_type;
};

#endif // VOLUME_H
