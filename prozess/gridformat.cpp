#include "gridformat.h"


#include<QMap>

namespace{

    QMap<GridFormat, QString> format_name_lookup{
        { GridFormat::XYZ, "X-Y-Z"},
        { GridFormat::ILXLZ, "Iline-Xline-Z"},
        { GridFormat::XYILXLZ, "X-Y-Iline-Xline-Z"},
    };
}


QString toQString(GridFormat f){
    return format_name_lookup.value(f, "X-Y-Z");
}

GridFormat toGridFormat(QString str){
    return format_name_lookup.key(str, GridFormat::XYZ);
}
