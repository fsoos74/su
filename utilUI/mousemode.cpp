#include "mousemode.h"

#include<QPixmap>
#include<QMap>

namespace{

QMap<MouseMode, QString> lookupName{

    { MouseMode::Explore, QString("Explore") },
    { MouseMode::Select, QString("Select") },
    { MouseMode::Zoom, QString("Zoom") },
    { MouseMode::Pick, QString("Pick") },
    { MouseMode::DeletePick, QString("Delete Pick") }

};


// pixmaps can only be created after qapplication
/*
QMap<MouseMode, QCursor> lookupCursor{

    { MouseMode::Explore, QCursor(Qt::ArrowCursor) },
    { MouseMode::Select, QCursor( QPixmap(":/icons/images/cross-cursor-select-24x24.png") ) },
    { MouseMode::Zoom, QCursor( QPixmap(":/icons/images/cross-cursor-zoom-24x24.png") ) },
    { MouseMode::Pick, QCursor(Qt::CrossCursor) },
    { MouseMode::DeletePick, QCursor(Qt::CrossCursor) }

};

QMap<MouseMode, QPixmap> lookupPixmap{

    { MouseMode::Explore, QPixmap(":/icons/images/arrow-32x32.png") },
    { MouseMode::Select, QPixmap(":/icons/images/select-32x32.png") },
    { MouseMode::Zoom, QPixmap(":/icons/images/Zoom.png") },
    { MouseMode::Pick, QPixmap(":/icons/images/pick_32x32.png") },
    { MouseMode::DeletePick, QPixmap(":/icons/images/delete_pick_32x32.png") }
};
*/
}

QString toQString( const MouseMode& o){

    return lookupName.value( o );
}

MouseMode toMouseMode(const QString& str){

    return lookupName.key(str, MouseMode::Explore);
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


QPixmap modePixmap( MouseMode m){

    switch(m){
    case MouseMode::Explore:
        return QPixmap(":/icons/images/arrow-32x32.png");
        break;
    case MouseMode::Select:
        return QPixmap(":/icons/images/select-32x32.png");
        break;
    case MouseMode::Zoom:
        return QPixmap(":/icons/images/Zoom.png");
        break;
    case MouseMode::Pick:
        return QPixmap(":/icons/images/pick_32x32.png");
        break;
    case MouseMode::DeletePick:
        return QPixmap(":/icons/images/delete_pick_32x32.png");
        break;
    }

    return QPixmap();
}

