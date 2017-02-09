#ifndef PICKMODE_H
#define PICKMODE_H

#include <QString>

enum class PickMode{
    Single,
    Left,
    Right,
    All
};


QString toQString(PickMode);
PickMode toPickMode(QString);

#endif // PICKMODE_H
