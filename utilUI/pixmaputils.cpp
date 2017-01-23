#include "pixmaputils.h"

#include<QPixmap>

#include <cmath>

#include <iostream>


// scales src (virtually) to virtSize and copies copyRect
QPixmap virtualScaledCopy( QPixmap src, QSize virtSize, QRect copyRect ){

    //std::cout<<"-------------------------------------"<<std::endl;
    //std::cout<<"src W="<<src.width()<<" H="<<src.height()<<std::endl;
    //std::cout<<"virt W="<<virtSize.width()<<" H="<<virtSize.height()<<std::endl;
    //std::cout<<"copyRect X="<<copyRect.x()<<" Y="<<copyRect.y()<<" W="<<copyRect.width()<<" H="<<copyRect.height()<<std::endl;

    qreal relX=qreal(copyRect.x())/virtSize.width();
    qreal relY=qreal(copyRect.y())/virtSize.height();
    qreal relW=qreal(copyRect.width())/virtSize.width();
    qreal relH=qreal(copyRect.height())/virtSize.height();
    //std::cout<<"rel X="<<relX<<" Y="<<relY<<" W="<<relW<<" H="<<relH<<std::endl;

    int tmpX=std::floor(src.width() * relX -1 );    // add 1 pix on left
    int tmpY=std::floor(src.height() * relY -1 );   // add 1 pix on top
    int tmpW=std::ceil(src.width() * relW + 2 );    // add 1 pix on right
    int tmpH=std::ceil(src.height() * relH + 2 );   // add 1 pix at bottom
    //std::cout<<"tmp X="<<tmpX<<" Y="<<tmpY<<" W="<<tmpW<<" H="<<tmpH<<std::endl;

    QPixmap tmp = src.copy( tmpX, tmpY, tmpW, tmpH );

    qreal facX = qreal( virtSize.width()) / src.width();
    qreal facY = qreal( virtSize.height()) / src.height();
    //std::cout<<"facX="<<facX<<" facY="<<facY<<std::endl;

    int scaledW = std::ceil( facX * tmpW );
    int scaledH = std::ceil( facY * tmpH );
    int scaledX = std::floor(tmpX*facX) +1 ;
    int scaledY = std::floor(tmpY*facY) +1;
    //std::cout<<"scaled X="<<scaledX<<" Y="<<scaledY<<" W="<<scaledW<<" H="<<scaledH<<std::endl;

    QPixmap scaled = tmp.scaled( scaledW, scaledH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

    copyRect.translate( -scaledX, -scaledY );

    //std::cout<<"copyRect X="<<copyRect.x()<<" Y="<<copyRect.y()<<" W="<<copyRect.width()<<" H="<<copyRect.height()<<std::endl;


    return scaled.copy( copyRect );
}

QPixmap scaledCopy( QPixmap src, QSize scaledSize, QRect copyRect ){

    QPixmap scaled = src.scaled( scaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

    return scaled.copy( copyRect );
}
