#ifndef VOLUMETYPE_H
#define VOLUMETYPE_H

#include<QString>

enum class VolumeType : qint16 { AVO, IVEL, AVEL, Other };


QList<VolumeType> volumeTypeList();
QStringList volumeTypeNames();
QString toQString(VolumeType);
VolumeType toVolumeType(QString);


#endif // VOLUMETYPE_H
