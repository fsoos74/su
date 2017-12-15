#ifndef XPPWRITER_H
#define XPPWRITER_H

#include<QXmlStreamWriter>

#include "processparams.h"


class XPPWriter
{
public:

    XPPWriter( const ProcessParams& pp );

    bool writeFile(QIODevice *device);

private:

    const ProcessParams& processParams;
    QXmlStreamWriter xml;
};

#endif // XPPWRITER_H
