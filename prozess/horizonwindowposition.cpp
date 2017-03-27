#include "horizonwindowposition.h"

#include<QMap>

namespace{

const QMap<HorizonWindowPosition, QString> lookup{
    {HorizonWindowPosition::Above, "Above" },
    {HorizonWindowPosition::Center, "Center"},
    {HorizonWindowPosition::Below, "Below"}
};

}

HorizonWindowPosition toHorizonWindowPosition( QString str){
    return lookup.key(str, HorizonWindowPosition::Center);
}

QString toQString( HorizonWindowPosition p){
    return lookup.value(p, lookup.value(HorizonWindowPosition::Center) );
}
