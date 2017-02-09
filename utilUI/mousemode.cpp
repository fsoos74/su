#include "mousemode.h"

#include<QPixmap>
#include<QMap>

namespace{

QMap<MouseMode, QString> lookup{

    { MouseMode::Explore, QString("Explore") },
    { MouseMode::Select, QString("Select") },
    { MouseMode::Zoom, QString("Zoom") },
    { MouseMode::Pick, QString("Pick") },
    { MouseMode::DeletePick, QString("Delete Pick") }

};

}

QString toQString( const MouseMode& o){

    return lookup.value( o, lookup.value(MouseMode::Explore) );
}

MouseMode toMouseMode(const QString& str){

    return lookup.key(str, MouseMode::Explore);
}


QDataStream& operator<<(QDataStream& out, const MouseMode& o){

    out<<toQString(o);

    return out;
}

QDataStream& operator>>(QDataStream& in, MouseMode& o){

    QString str;

    in>>str;

    o=toMouseMode(str);     // will return explore for invalid str

    return in;
}


QCursor modeCursor( MouseMode m){

    switch(m){
    case MouseMode::Explore:
        //pm=QPixmap(":/icons/images/cross-cursor-24x24.png");
        return QCursor(Qt::ArrowCursor);
        break;
    case MouseMode::Select:
        return QCursor( QPixmap(":/icons/images/cross-cursor-select-24x24.png") );
        break;
    case MouseMode::Zoom:
        return QCursor( QPixmap(":/icons/images/cross-cursor-zoom-24x24.png") );
        break;
    case MouseMode::Pick:
        //pm=QPixmap(":/icons/images/cross-cursor-24x24.png");
        return QCursor(Qt::CrossCursor);
        break;
    case MouseMode::DeletePick:
        //pm=QPixmap(":/icons/images/cross-cursor-24x24.png");
        return QCursor(Qt::CrossCursor);
        break;
    }

    return QCursor();
}
