#ifndef GRIDTYPE_H
#define GRIDTYPE_H

#include <QString>

/*
namespace{
enum class GridType : qint16{ Other, AVO, HRZ, VEL };
}
*/

enum class GridType { Horizon, Attribute, Other };

QList<GridType> gridTypes();
QStringList gridTypeNames();
QString toQString(GridType);
GridType toGridType(QString);

#endif // GRIDTYPE_H
