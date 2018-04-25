#include "zmode.h"
#include<QMap>

namespace {
QMap<ZMode,QString> zmodeLookup{
    {ZMode::MD, "MD"},
    {ZMode::TVD, "TVD"}
};
}

QString toQString(ZMode m){
    return zmodeLookup.value(m, "MD");
}

ZMode toZMode(QString str){
    return zmodeLookup.key(str, ZMode::MD);
}

QList<ZMode> zmodes(){
    return zmodeLookup.keys();
}

QStringList zmodeNames(){
    return zmodeLookup.values();
}
