#include "xctwriter.h"

#include "colortable.h"

XCTWriter::XCTWriter( QVector<QRgb>& colors ) : m_colors( colors)
{
    xml.setAutoFormatting(true);
}

bool XCTWriter::writeFile(QIODevice *device)
{

    Q_ASSERT( device);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE colortable>");

    xml.writeStartElement("xct");
    xml.writeAttribute("version", "1.0");

    xml.writeStartElement("colors");
    xml.writeAttribute("size", QString::number(m_colors.size()));

    for (int i = 0; i < m_colors.size(); ++i){
        writeColor( i, m_colors[i]);
    }

    xml.writeEndElement();

    xml.writeEndDocument();

    return true;
}

void XCTWriter::writeColor( int idx, const QRgb& rgb)
{
    const QString tagName = "color";

    xml.writeStartElement(tagName);

    xml.writeAttribute("index", QString::number(idx));

    xml.writeAttribute("red", QString::number(qRed(rgb)));
    xml.writeAttribute("green", QString::number(qGreen(rgb)));
    xml.writeAttribute("blue", QString::number(qBlue(rgb)));
    xml.writeAttribute("alpha", QString::number(qAlpha(rgb)));

    xml.writeEndElement();
}

