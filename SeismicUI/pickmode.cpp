#include "pickmode.h"

#include <QMap>

namespace{


QMap< PickMode, QString> lookup{

    { PickMode::Single, QString("Single") },
    { PickMode::Left, QString("Left") },
    { PickMode::Right, QString("Right") },
    { PickMode::Lines, QString("Lines") },
    { PickMode::Outline, QString("Outline") }
};

}

QString toQString(PickMode m){

    Q_ASSERT( lookup.contains(m));

    return lookup.value(m, lookup.value(PickMode::Single) );
}

PickMode toPickMode(QString str){

    return lookup.key( str, PickMode::Single );
}

QPixmap pickModePixmap(PickMode t){

    switch(t){
    case PickMode::Single:
        return QPixmap(":/icons/images/pick-single-32x32.png");
        break;
    case PickMode::Left:
        return QPixmap(":/icons/images/pick-left-32x32.png");
        break;
    case PickMode::Right:
        return QPixmap(":/icons/images/pick-right-32x32.png");
        break;
    case PickMode::Lines:
        return QPixmap(":/icons/images/pick-lines-32x32.png");
        break;
    case PickMode::Outline:
        return QPixmap(":/icons/images/pick-outline-32x32.png");
        break;
   }

    return QPixmap();
}
