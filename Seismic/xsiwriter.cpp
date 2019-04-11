#include "xsiwriter.h"


namespace seismic{



XSIWriter::XSIWriter(const seismic::SEGYInfo& info):m_info(info)
{
    xml.setAutoFormatting(true);
}


bool XSIWriter::writeFile(QIODevice *device){

    Q_ASSERT( device);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE xsi>");

    xml.writeStartElement("SEGYInfo");
    xml.writeAttribute("version", "1.0");

    xml.writeAttribute("swap", (m_info.isSwap())? "true":"false");
    xml.writeAttribute("override-fixed-sampling", (m_info.isOverrideFixedSampling())? "true":"false");
    xml.writeAttribute("fixed-sampling", (m_info.isFixedSampling())? "true":"false");

    QString format="header";

    if( m_info.isOverrideSampleFormat() ){

        format=toQString(m_info.sampleFormat());
    }
    xml.writeAttribute("format", format);

    QString next=(m_info.isOverrideNEXT())?QString::number(m_info.next()) : "header";
    xml.writeAttribute("next", next);

    QString scalco=(m_info.isFixedScalco())?QString::number(m_info.scalco()) : "header";
    xml.writeAttribute("scalco", scalco);

    QString scalel=(m_info.isFixedScalel())?QString::number(m_info.scalel()) : "header";
    xml.writeAttribute("scalel", scalel);

    xml.writeStartElement("binary-header-def");
    for( auto h : m_info.binaryHeaderDef()){
        writeHeaderWordDef(h);
    }
    xml.writeEndElement();


    xml.writeStartElement("trace-header-def");
    for( auto h : m_info.traceHeaderDef()){
        writeHeaderWordDef(h);
    }
    xml.writeEndElement();





    xml.writeEndDocument();

    return true;
}


void XSIWriter::writeHeaderWordDef(const seismic::SEGYHeaderWordDef & def){

    QString dtype=toQString(def.dtype);

    QString ctype=toQString(def.ctype);

    xml.writeStartElement("word");
    xml.writeAttribute("name", def.name.c_str());
    xml.writeAttribute("dtype", dtype);
    xml.writeAttribute("ctype", ctype);
    xml.writeAttribute("pos", QString::number(def.pos));
    xml.writeAttribute("descr", QString(def.descr.c_str()));
    xml.writeEndElement();

}


}
