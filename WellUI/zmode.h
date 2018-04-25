#ifndef ZMODE_H
#define ZMODE_H

#include<QString>
#include<QList>

enum class ZMode{ MD, TVD };

QString toQString(ZMode);
ZMode toZMode(QString);
QList<ZMode> zmodes();
QStringList zmodeNames();

#endif // ZMODE_H
