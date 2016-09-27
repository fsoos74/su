#ifndef AXXISORIENTATION_H
#define AXXISORIENTATION_H

#include<QVariant>
#include<QDataStream>
#include<QString>

enum class AxxisOrientation{ Horizontal, Vertical };
enum class AxxisDirection{ Ascending, Descending };

Q_DECLARE_METATYPE(AxxisOrientation);
Q_DECLARE_METATYPE(AxxisDirection);

QString toQString( const AxxisOrientation&);
AxxisOrientation toAxxisOrientation(const QString&);

QString toQString( const AxxisDirection&);
AxxisDirection toAxxisDirection(const QString&);

QDataStream& operator<<(QDataStream& out, const AxxisOrientation& o);
QDataStream& operator>>(QDataStream& in, AxxisOrientation& o);

QDataStream& operator<<(QDataStream& out, const AxxisDirection& d);
QDataStream& operator>>(QDataStream& in, AxxisDirection& d);



#endif // AXXISORIENTATION_H

