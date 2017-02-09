#include "picktype.h"

#include<QMap>

namespace{

QMap<PickType, QString> lookup{

    { PickType::Free, QString("Free") },
    { PickType::Zero, QString("Zero") },
    { PickType::Minimum, QString("Minimum") },
    { PickType::Maximum, QString("Maximum") }
};

}



QString toQString(PickType t){

    Q_ASSERT( lookup.contains(t));

    return lookup.value(t, lookup.value(PickType::Free));
}


PickType toPickType( QString str ){

    return lookup.key( str, PickType::Free);
}
