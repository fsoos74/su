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

QPixmap pickTypePixmap(PickType t){

    switch(t){
    case PickType::Minimum:
        return QPixmap(":/icons/images/pick-minimum-32x32.png");
        break;
    case PickType::Maximum:
        return QPixmap(":/icons/images/pick-maximum-32x32.png");
        break;

    case PickType::Zero:
        return QPixmap(":/icons/images/pick-zero-32x32.png");
        break;

    case PickType::Free:
        return QPixmap(":/icons/images/pick-free-32x32.png");
        break;
    }

    return QPixmap();
}
