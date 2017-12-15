#include "xdireader.h"
#include<iostream>
XDIReader::XDIReader( SeismicDatasetInfo& info) : m_info( info )
{
}

bool XDIReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "xdi" ){ //%%&& xml.attributes().value("version") == "1.0"){

            while(xml.readNextStartElement()){
//std::cout<<xml.name().toString().toStdString()<<std::endl<<std::flush;
                bool num_ok=true;

                if( xml.name()=="name"){
                    m_info.setName(xml.readElementText());
                }
                else  if( xml.name()=="path"){
                    m_info.setPath(xml.readElementText());
                }
                else  if( xml.name()=="info-path"){
                    m_info.setInfoPath(xml.readElementText());
                }
                else  if( xml.name()=="index-path"){
                    m_info.setIndexPath(xml.readElementText());
                }
                else if( xml.name()=="domain"){
                    m_info.setDomain( stringToDatasetDomain( xml.readElementText() ) );
                }
                else if( xml.name()=="mode"){
                    m_info.setMode( stringToDatasetMode( xml.readElementText() ) );
                }
                else if( xml.name()=="ft"){
                    m_info.setFT( xml.readElementText().toDouble(&num_ok) );
                }
                else if( xml.name()=="dt"){
                    m_info.setDT( xml.readElementText().toDouble(&num_ok) );
                }
                else if( xml.name()=="nt"){
                    m_info.setNT( xml.readElementText().toULong(&num_ok) );
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
            xml.raiseError(QObject::tr("The file is not an xpr version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XDIReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}
