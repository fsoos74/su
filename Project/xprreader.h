#ifndef XPRREADER_H
#define XPRREADER_H


#include <QXmlStreamReader>
#include "avoproject.h"

class XPRReader
{
public:

    XPRReader(AVOProject*, QString projectFilePath );

    bool read(QIODevice *device);

    QString errorString() const;

private:

    void readGeometry();
    void readOrientation();


    QXmlStreamReader xml;
    AVOProject* m_project;
    QString m_projectFilePath;
};


#endif // XPRREADER_H
