#ifndef HHORIZONDEF_H
#define HHORIZONDEF_H


#include<memory>
#include<grid2d.h>
#include<QColor>


struct HorizonDef{
    std::shared_ptr<Grid2D<float>> horizon;
    QString volume;								// used for color mapping if set
    int delay;                                  // milliseconds
    QColor color;					    // used when no volumes specified
};

inline bool operator==(const HorizonDef& h1, const HorizonDef& h2){

    return (h1.horizon == h2.horizon ) &&
            (h1.volume == h2.volume) &&
            (h1.delay == h2.delay) &&
            (h1.color == h2.color);
}


#endif // HORIZONDEF_H
