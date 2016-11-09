#ifndef XPRREADER_H
#define XPRREADER_H


#include <QXmlStreamReader>
#include "avoproject.h"

class XPRReader
{
public:

    XPRReader(AVOProject&, QString projectFilePath );

    bool read(QIODevice *device);

    QString errorString() const;

private:

    void readGeometry();
    void readOrientation();

    QString m_projectFilePath;

    QXmlStreamReader xml;
    AVOProject& m_project;
};


#endif // XPRREADER_H
