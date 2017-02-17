#include "pickfillmode.h"

#include <QMap>

namespace{


QMap< PickFillMode, QString> lookup{

    { PickFillMode::Next, QString("Next") },
    { PickFillMode::Nearest, QString("Nearest") }
};

}

QString toQString(PickFillMode m){

    Q_ASSERT( lookup.contains(m));

    return lookup.value(m, lookup.value(PickFillMode::Next) );
}

PickFillMode toPickFillMode(QString str){

    return lookup.key( str, PickFillMode::Next );
}
