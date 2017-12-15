#ifndef XWIREADER_H
#define XWIREADER_H

#include <QXmlStreamReader>
#include "wellinfo.h"

class XWIReader
{
public:

    XWIReader(WellInfo&);

    bool read(QIODevice *device);

    QString errorString() const;

private:


    QXmlStreamReader xml;
    WellInfo& m_info;
};

#endif // XWIREADER_H
