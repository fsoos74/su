#include "xgrwriter.h"
#include "grid2d.h"

#include<iostream>

XGRWriter::XGRWriter( const Grid2D<float>& grid ) : m_grid( grid)
{
    xml.setAutoFormatting(true);
}

bool XGRWriter::writeFile(QIODevice *device)
{

    Q_ASSERT( device);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE xgr>");

    xml.writeStartElement("grid");
    xml.writeAttribute("version", "1.0");

    xml.writeStartElement("bounds");
    xml.writeAttribute("j1", QString::number(m_grid.bounds().j1()));
    xml.writeAttribute("i1", QString::number(m_grid.bounds().i1()));
    xml.writeAttribute("j2", QString::number(m_grid.bounds().j2()));
    xml.writeAttribute("i2", QString::number(m_grid.bounds().i2()));
    xml.writeEndElement();

    xml.writeStartElement("data");
    xml.writeAttribute("null", QString::number(m_grid.NULL_VALUE));

    for( int i=m_grid.bounds().i1(); i<=m_grid.bounds().i2(); i++){

        for( int j=m_grid.bounds().j1(); j<=m_grid.bounds().j2(); j++){

            if( m_grid(i,j)==m_grid.NULL_VALUE) continue;

            xml.writeStartElement("point");
            xml.writeAttribute("i", QString::number(i));
            xml.writeAttribute("j", QString::number(j));
            xml.writeAttribute("value", QString::number(m_grid(i,j)));
            xml.writeEndElement();
        }

    }


    xml.writeEndElement();

    xml.writeEndDocument();

    return true;
}
