#ifndef HHORIZONDEF_H
#define HHORIZONDEF_H


#include<memory>
#include<grid2d.h>
#include<QColor>


// all members have default constructors
struct HorizonDef{
    std::shared_ptr<Grid2D<float>> horizon;
    bool useColor;
    QColor color;
    qreal colorScaler;
    int delay;                                  // milliseconds
};

inline bool operator==(const HorizonDef& h1, const HorizonDef& h2){

    return (h1.horizon == h2.horizon ) &&
            (h1.useColor == h2.useColor ) &&
            (h1.color == h2.color ) &&
            (h1.colorScaler == h2.colorScaler ) &&
            (h1.delay == h2.delay);
}


#endif // HORIZONDEF_H
