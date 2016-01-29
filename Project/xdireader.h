#ifndef XDIREADER_H
#define XDIREADER_H


#include <QXmlStreamReader>
#include "seismicdatasetinfo.h"

class XDIReader
{
public:

    XDIReader(SeismicDatasetInfo&);

    bool read(QIODevice *device);

    QString errorString() const;

private:


    QXmlStreamReader xml;
    SeismicDatasetInfo& m_info;
};

#endif // XDIREADER_H
