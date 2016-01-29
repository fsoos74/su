#ifndef XDIWRITER_H
#define XDIWRITER_H


#include<QXmlStreamWriter>

#include"seismicdatasetinfo.h"


class XDIWriter
{
public:

    XDIWriter( const SeismicDatasetInfo& info);

    bool writeFile(QIODevice *device);

private:

    const SeismicDatasetInfo& m_info;
    QXmlStreamWriter xml;
};


#endif // XDIWRITER_H
