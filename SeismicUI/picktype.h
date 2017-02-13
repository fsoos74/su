#ifndef PICKTYPE_H
#define PICKTYPE_H

#include <QString>
#include <QPixmap>

enum class PickType{
    Free,
    Minimum,
    Maximum,
    Zero
};


QString toQString(PickType);

PickType toPickType(QString);

QPixmap pickTypePixmap(PickType);

#endif // PICKTYPE_H
