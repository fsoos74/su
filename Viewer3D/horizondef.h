#ifndef HHORIZONDEF_H
#define HHORIZONDEF_H


#include<memory>
#include<grid2d.h>
#include<QColor>


// all members have default constructors
struct HorizonDef{
    std::shared_ptr<Grid2D<float>> horizon;
    QString volume;
    int delay;                                  // milliseconds
};

inline bool operator==(const HorizonDef& h1, const HorizonDef& h2){

    return (h1.horizon == h2.horizon ) &&
            (h1.volume == h2.volume) &&
            (h1.delay == h2.delay);
}


#endif // HORIZONDEF_H
