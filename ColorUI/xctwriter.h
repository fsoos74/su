#ifndef XCTWriter_H
#define XCTWriter_H

#include<QXmlStreamWriter>
#include<QVector>
#include<QRgb>

class ColorTable;



class XCTWriter
{
public:

    XCTWriter( QVector<QRgb>& colors);

    bool writeFile(QIODevice *device);

private:

    void writeColor( int idx, const QRgb& rgb);

    QVector<QRgb>& m_colors;
    QXmlStreamWriter xml;
};

#endif // XCTWriter_H
