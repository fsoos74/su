#include "xdiwriter.h"


XDIWriter::XDIWriter( const SeismicDatasetInfo& info):m_info(info){
    xml.setAutoFormatting(true);
}

bool XDIWriter::writeFile(QIODevice *device){
    Q_ASSERT( device);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE seismic-dataset-info>");

    xml.writeStartElement("xdi");
    xml.writeAttribute("version", "1.1");

    xml.writeTextElement("name", m_info.name());
    xml.writeTextElement("path", m_info.path());
    xml.writeTextElement("info-path", m_info.infoPath());
    xml.writeTextElement("index-path", m_info.indexPath());
    xml.writeTextElement("dimensions", QString::number(m_info.dimensions()));
    xml.writeTextElement("domain", datasetDomainToString( m_info.domain() ) );
    xml.writeTextElement("mode", datasetModeToString( m_info.mode() ) );
    xml.writeTextElement("ft", QString::number(m_info.ft()));
    xml.writeTextElement("dt", QString::number(m_info.dt()));
    xml.writeTextElement("nt", QString::number(m_info.nt()));
    xml.writeEndElement();

    xml.writeEndDocument();

    return !(xml.hasError());
}
