#include "axxisorientation.h"





QString toQString( const AxxisOrientation& o){

    return QString( (o==AxxisOrientation::Horizontal) ? "Horizontal" : "Vertical" );
}

AxxisOrientation toAxxisOrientation(const QString& str){

    AxxisOrientation o;

    if( str=="Horizontal"){
        o=AxxisOrientation::Horizontal;
    }
    else {
        o=AxxisOrientation::Vertical;
    }

    return o;
}

QString toQString( const AxxisDirection& d){

    return QString( (d==AxxisDirection::Ascending) ? "Ascending" : "Descending" );
}

AxxisDirection toAxxisDirection(const QString& str){

    AxxisDirection d;

    if( str=="Ascending"){
        d=AxxisDirection::Ascending;
    }
    else {
        d=AxxisDirection::Descending;
    }

    return d;
}


QDataStream& operator<<(QDataStream& out, const AxxisOrientation& o){

    out<<toQString(o);

    return out;
}

QDataStream& operator>>(QDataStream& in, AxxisOrientation& o){

    QString str;

    in>>str;

    o=toAxxisOrientation(str);

    return in;
}

QDataStream& operator<<(QDataStream& out, const AxxisDirection& d){

    out<<toQString(d);

    return out;
}

QDataStream& operator>>(QDataStream& in, AxxisDirection& d){

    QString str;

    in>>str;

    d=toAxxisDirection(str);

    return in;
}


