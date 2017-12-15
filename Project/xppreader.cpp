#include "xppreader.h"

XPPReader::XPPReader( ProcessParams& pp ) : processParams(pp)
{

}


bool XPPReader::readFile(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "xpp" && xml.attributes().value("version") == "1.0"){

            while(xml.readNextStartElement()){

                processParams.insert( xml.name().toString(), xml.readElementText() );
            }
        }
        else{
            xml.raiseError(QObject::tr("The file is not an xpr version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XPPReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}
