#ifndef VOLUMETYPE_H
#define VOLUMETYPE_H

#include<QString>

enum class VolumeType : qint16 { AVO, VEL, Other };


QList<VolumeType> volumeTypeList();
QStringList volumeTypeNames();
QString toQString(VolumeType);
VolumeType toVolumeType(QString);


#endif // VOLUMETYPE_H
