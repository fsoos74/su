#include "xwireader.h"

#include "xwireader.h"
#include<iostream>


XWIReader::XWIReader( WellInfo& info) : m_info( info )
{
}

bool XWIReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "xwi" ){ //%%&& xml.attributes().value("version") == "1.0"){

            while(xml.readNextStartElement()){
//std::cout<<xml.name().toString().toStdString()<<std::endl<<std::flush;
                bool num_ok=true;

                if( xml.name()=="name"){
                    m_info.setName(xml.readElementText() );
                }
                else  if( xml.name()=="uwi"){
                    m_info.setUWI(xml.readElementText() );
                }
                else if( xml.name()=="x"){
                    m_info.setX( xml.readElementText().toDouble(&num_ok) );
                }
                else if( xml.name()=="y"){
                    m_info.setY( xml.readElementText().toDouble(&num_ok) );
                }
                else if( xml.name()=="z"){
                    m_info.setZ( xml.readElementText().toDouble(&num_ok) );
                }
                else{
                    xml.raiseError("Unexpected element");
                }

                if(!num_ok){
                    xml.raiseError(QString("Invalid number") );
                }
            }
        }
        else{
            xml.raiseError(QObject::tr("The file is not an xwi version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XWIReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}
