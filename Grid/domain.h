#ifndef DOMAIN_H
#define DOMAIN_H

#include<QString>

enum class Domain : qint16{ Other, Time, Depth };

QStringList domainNames();
QList<Domain> domainList();
QString toQString(Domain);
Domain toDomain(QString);

#endif // DOMAIN_H
