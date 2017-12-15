#ifndef VOLUMEDIPTRACKER_H
#define VOLUMEDIPTRACKER_H

#include <volume.h>
#include <memory>


class VolumeDipTracker
{
public:
    VolumeDipTracker( std::shared_ptr<Volume> ildip, std::shared_ptr<Volume> xldip );

    double track( int i, int j, double t, int ii, int jj)const;

private:

    std::shared_ptr<Volume> m_ildip;
    std::shared_ptr<Volume> m_xldip;
};

#endif // VOLUMEDIPTRACKER_H
