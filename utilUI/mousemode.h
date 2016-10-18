#ifndef MOUSEMODE_H
#define MOUSEMODE_H

#include<QVariant>
#include<QDataStream>
#include<QString>

#include<QCursor>

enum class MouseMode{ Explore, Select, Zoom };

Q_DECLARE_METATYPE(MouseMode);


QString toQString( const MouseMode&);
MouseMode toMousMode(const QString&);


QDataStream& operator<<(QDataStream& out, const MouseMode& o);
QDataStream& operator>>(QDataStream& in, MouseMode& o);


QCursor modeCursor( MouseMode );

#endif // MOUSEMODE_H
