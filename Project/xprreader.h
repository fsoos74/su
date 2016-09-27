#ifndef XPRREADER_H
#define XPRREADER_H


#include <QXmlStreamReader>
#include "avoproject.h"

class XPRReader
{
public:

    XPRReader(AVOProject&);

    bool read(QIODevice *device);

    QString errorString() const;

private:

    void readGeometry();
    void readOrientation();

    QXmlStreamReader xml;
    AVOProject& m_project;
};


#endif // XPRREADER_H
