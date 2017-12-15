#include "xwiwriter.h"

#include "xwiwriter.h"


XWIWriter::XWIWriter( const WellInfo& info):m_info(info){
    xml.setAutoFormatting(true);
}

bool XWIWriter::writeFile(QIODevice *device){
    Q_ASSERT( device);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE well-info>");

    xml.writeStartElement("xwi");
    xml.writeAttribute("version", "1.0");

    xml.writeTextElement("name", m_info.name());
    xml.writeTextElement("uwi", m_info.uwi());
    xml.writeTextElement("x", QString::number(m_info.x()));
    xml.writeTextElement("y", QString::number(m_info.y()));
    xml.writeTextElement("z", QString::number(m_info.z()));
    xml.writeEndElement();

    xml.writeEndDocument();

    return !(xml.hasError());
}

