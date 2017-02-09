#ifndef PICKTYPE_H
#define PICKTYPE_H

#include <QString>


enum class PickType{
    Free,
    Minimum,
    Maximum,
    Zero
};


QString toQString(PickType);

PickType toPickType(QString);


#endif // PICKTYPE_H
