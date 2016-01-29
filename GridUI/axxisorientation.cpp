#include "axxisorientation.h"


QDataStream& operator<<(QDataStream& out, const AxxisOrientation& o){

    out<<QString( (o==AxxisOrientation::Horizontal) ? "Horizontal" : "Vertical" );

    return out;
}

QDataStream& operator>>(QDataStream& in, AxxisOrientation& o){

    QString str;

    in>>str;

    if( str=="Horizontal"){
        o=AxxisOrientation::Horizontal;
    }
    else {
        o=AxxisOrientation::Vertical;
    }

    return in;
}

QDataStream& operator<<(QDataStream& out, const AxxisDirection& d){

    out<<QString( (d==AxxisDirection::Ascending) ? "Ascending" : "Descending" );

    return out;
}

QDataStream& operator>>(QDataStream& in, AxxisDirection& d){

    QString str;

    in>>str;

    if( str=="Ascending"){
        d=AxxisDirection::Ascending;
    }
    else {
        d=AxxisDirection::Descending;
    }

    return in;
}


