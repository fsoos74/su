#include"domain.h"

#include<QMap>

namespace{
QMap<Domain, QString> lookup{
    {Domain::Other, "Other"},
    {Domain::Time, "Time"},
    {Domain::Depth, "Depth"}
};
}


QList<Domain> domainList(){
    return lookup.keys();
}

QStringList domainNames(){
    return lookup.values();
}

QString toQString(Domain domain){
    return lookup.value(domain, "Other");
}


Domain toDomain(QString str){
    return lookup.key(str, Domain::Other);
}
