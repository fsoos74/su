#include "xctreader.h"

#include<iostream>

XCTReader::XCTReader( QVector<QRgb>& colors) : m_colors(colors)
{
}

bool XCTReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "xct" && xml.attributes().value("version") == "1.0"){

            readColors();
        }
        else{
            xml.raiseError(QObject::tr("The file is not an xct version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XCTReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}


void XCTReader::readColors(){

    while( xml.readNextStartElement() && xml.name()!="colors"){

       qWarning("Skipping element");
       xml.skipCurrentElement();
    }

    if( xml.name()!="colors"){
        qWarning("Nothing found!");
        return;
    }

    int size=xml.attributes().value("size").toInt();

    if( size<0 ){

        xml.raiseError("Illegal number of colors!");
        return;
    }

    m_colors.resize(size);

    while( xml.readNextStartElement()){

        if( xml.name()=="color"){
            readColor();
        }
        else{
            xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
    }
 }

void XCTReader::readColor(){

    int idx=xml.attributes().value("index").toInt();
    int red=xml.attributes().value("red").toInt();
    int green=xml.attributes().value("green").toInt();
    int blue=xml.attributes().value("blue").toInt();
    int alpha=255;
    if(xml.attributes().hasAttribute("alpha")){
        alpha=xml.attributes().value("alpha").toInt();
    }
    if( idx<0 || idx>=m_colors.size()){
        xml.raiseError(QObject::tr("Illegal index for color!"));
        return;
    }

    xml.skipCurrentElement();

    m_colors[idx]=qRgba(red, green, blue, alpha);


}

