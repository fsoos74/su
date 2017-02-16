#ifndef PICKMODE_H
#define PICKMODE_H

#include <QString>
#include <QPixmap>

enum class PickMode{
    Single,
    Left,
    Right
};


QString toQString(PickMode);
PickMode toPickMode(QString);

QPixmap pickModePixmap(PickMode);

#endif // PICKMODE_H
