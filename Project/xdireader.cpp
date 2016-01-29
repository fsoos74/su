#include "xdireader.h"

XDIReader::XDIReader( SeismicDatasetInfo& info) : m_info( info )
{
}

bool XDIReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "xdi" && xml.attributes().value("version") == "1.0"){

            while(xml.readNextStartElement()){
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
                else{
                    xml.raiseError("Unexpected element");
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
