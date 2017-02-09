#include "pickmode.h"

#include <QMap>

namespace{


QMap< PickMode, QString> lookup{

    { PickMode::Single, QString("Single") },
    { PickMode::Left, QString("Left") },
    { PickMode::Right, QString("Right") },
    { PickMode::All, QString("All") }
};

}

QString toQString(PickMode m){

    Q_ASSERT( lookup.contains(m));

    return lookup.value(m, lookup.value(PickMode::Single) );
}

PickMode toPickMode(QString str){

    return lookup.key( str, PickMode::Single );
}
