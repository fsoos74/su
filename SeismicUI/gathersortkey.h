#ifndef GATHERSORTKEY_H
#define GATHERSORTKEY_H

#include<QVariant>
#include<QDataStream>
#include<QString>

enum class GatherSortKey{ None, Inline, Crossline, Offset };

Q_DECLARE_METATYPE(GatherSortKey);

QString toQString( const GatherSortKey&);
GatherSortKey toGatherSortKey(const QString&);

QDataStream& operator<<(QDataStream& out, const GatherSortKey&);
QDataStream& operator>>(QDataStream& in, GatherSortKey&);


#endif // GATHERSORTKEY_H
