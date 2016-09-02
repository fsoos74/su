#include "xgrreader.h"

#include<iostream>

XGRReader::XGRReader( Grid2D<float>& grid) : m_grid(grid)
{
}

bool XGRReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "grid" && xml.attributes().value("version") == "1.0"){

            readXGR();
        }
        else{
            xml.raiseError(QObject::tr("The file is not an xgr version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XGRReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}


void XGRReader::readXGR(){

    xml.readNextStartElement();
    if(xml.name()!="bounds"){
        xml.raiseError("Unexpected tag, expected bounds!");
        return;
    }

    int i1=xml.attributes().value("i1").toInt();
    int i2=xml.attributes().value("i2").toInt();
    int j1=xml.attributes().value("j1").toInt();
    int j2=xml.attributes().value("j2").toInt();
    m_grid=Grid2D<float>( Grid2DBounds( i1, j1, i2, j2), Grid2D<float>::NULL_VALUE );
    xml.skipCurrentElement();

    xml.readNextStartElement();
    if(xml.name()!="data"){
        xml.raiseError("Unexpected tag, expected data!");
        return;
    }

    while( xml.readNextStartElement()){

        if( xml.name()=="point"){
            readPoint();
        }
        else{
            xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
    }
}

void XGRReader::readPoint(){

    int i=xml.attributes().value("i").toInt();
    int j=xml.attributes().value("j").toInt();
    double value=xml.attributes().value("value").toDouble();

    if( ! m_grid.bounds().isInside( i, j) ){
        std::cout<<"i="<<i<<" j="<<j<<" i1="<<m_grid.bounds().i1()<<" i2="<<m_grid.bounds().i2();
        std::cout<<" j1="<<m_grid.bounds().j1()<<" j2="<<m_grid.bounds().j2()<<std::endl<<std::flush;
        xml.raiseError("Point is outside grid bounds!");
        return;
    }

    m_grid(i,j)=value;

    xml.skipCurrentElement();
}

