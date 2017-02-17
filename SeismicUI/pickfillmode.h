#ifndef PICKFILLMODE_H
#define PICKFILLMODE_H

#include<QString>

enum class PickFillMode{

    Next,
    Nearest
};

QString toQString(PickFillMode);

PickFillMode toPickFillMode(QString);


#endif // PICKFILLMODE_H
