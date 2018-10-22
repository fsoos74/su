#ifndef XMLPWRITER_H
#define XMLPWRITER_H

#include<QXmlStreamWriter>
#include<QStringList>
#include "simplemlp.h"


class XMLPWriter
{
public:
    using MLP=SimpleMLP;

    XMLPWriter(const MLP&, const QStringList& inames);

    bool writeFile(QIODevice *device);

private:

    const MLP& mlp;
    const QStringList& inames;
    QXmlStreamWriter xml;
};



#endif // XMLPWRITER_H
