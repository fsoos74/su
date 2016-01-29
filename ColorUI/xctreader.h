#ifndef XCTReader_H
#define XCTReader_H

#include <QXmlStreamReader>
#include <QVector>
#include <QRgb>
#include <QString>

class XCTReader
{
public:

    XCTReader(QVector<QRgb>& colors);

    bool read(QIODevice *device);

    QString errorString() const;

private:

    void readColors( );
    void readColor();

    QXmlStreamReader xml;
    QVector<QRgb>& m_colors;
};




#endif // XCTReader_H
