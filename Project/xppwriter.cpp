#include "xppwriter.h"


XPPWriter::XPPWriter(const ProcessParams& pp):processParams(pp)
{

}


bool XPPWriter::writeFile(QIODevice *device){
    Q_ASSERT( device);

    xml.setAutoFormatting(true);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE process-parameters>");

    xml.writeStartElement("xpp");
    xml.writeAttribute("version", "1.0");

    for( auto name : processParams.keys() ){

        xml.writeTextElement( name, processParams.value(name));
    }

    xml.writeEndElement();

    xml.writeEndDocument();

    return !(xml.hasError());
}
