#include "mousemode.h"

#include<QPixmap>


const QString EXPLORE_STR("Explore");
const QString SELECT_STR("Select");
const QString ZOOM_STR("Zoom");
const QString PICK_STR("Pick");

QString toQString( const MouseMode& o){

    switch(o){
    case MouseMode::Explore: return EXPLORE_STR; break;
    case MouseMode::Select: return SELECT_STR; break;
    case MouseMode::Zoom: return ZOOM_STR; break;
    case MouseMode::Pick: return PICK_STR; break;
    }

    // maye etter throw
    return QString("INVALID");
}

MouseMode toMouseMode(const QString& str){

    if( str==SELECT_STR ){
        return MouseMode::Select;
    }
    else if( str==ZOOM_STR){
        return MouseMode::Zoom;
    }
    else if( str==PICK_STR){
        return MouseMode::Pick;
    }
    else{
        return MouseMode::Explore;
    }
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
    }

    return QCursor();
}
