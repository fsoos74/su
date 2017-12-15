#ifndef GRIDFORMAT_H
#define GRIDFORMAT_H

#include<QString>

enum class GridFormat{ XYZ, ILXLZ, XYILXLZ };

QString toQString(GridFormat);
GridFormat toGridFormat(QString);


#endif // GRIDFORMAT_H
