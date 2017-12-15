#ifndef XWIWRITER_H
#define XWIWRITER_H


#include<QXmlStreamWriter>

#include"wellinfo.h"


class XWIWriter
{
public:

    XWIWriter( const WellInfo& info);

    bool writeFile(QIODevice *device);

private:

    const WellInfo& m_info;
    QXmlStreamWriter xml;
};


#endif // XWIWRITER_H
