#include "volumetype.h"

#include<QMap>

namespace{
QMap<VolumeType, QString> lookup{

    { VolumeType::Other, "Other"},
    { VolumeType::AVO, "AVO"},
    { VolumeType::IVEL, "Interval Velocity"},
    { VolumeType::AVEL, "Average Velocity"},

};
}


QList<VolumeType> volumeTypeList(){
    return lookup.keys();
}

QStringList volumeTypeNames(){
    return lookup.values();
}

QString toQString(VolumeType t){
    return lookup.value(t, "Other");
}

VolumeType toVolumeType(QString str){
    return lookup.key(str, VolumeType::Other);
}
