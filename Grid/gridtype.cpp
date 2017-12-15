#include "gridtype.h"

#include<QMap>

namespace{
/*
QMap<GridType, QString> lookup{
    {GridType::Other, "Other"},
    {GridType::AVO, "AVO"},
    {GridType::HRZ, "Horizon"}
};
*/
QMap<GridType, QString> lookup{
    {GridType::Other, "Other"},
    {GridType::Attribute, "Attribute"},
    {GridType::Horizon, "Horizon"}
};
}

QList<GridType> gridTypes(){
    return lookup.keys();
}

QStringList gridTypeNames(){
    return lookup.values();
}

QString toQString(GridType t){
    return lookup.value(t, "Other");
}


GridType toGridType(QString str){
    return lookup.key(str, GridType::Other);
}

