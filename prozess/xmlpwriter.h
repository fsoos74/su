#ifndef XMLPWRITER_H
#define XMLPWRITER_H

#include<QXmlStreamWriter>
#include<QStringList>
#include "simplemlp.h"


class XMLPWriter
{
public:
    using MLP=SimpleMLP;

    XMLPWriter(const MLP&, const QStringList& inames, int ilaper, int xlaper);

    bool writeFile(QIODevice *device);

private:

    const MLP& mlp;
    const QStringList& inames;
    int ilAperture;
    int xlAperture;
    QXmlStreamWriter xml;
};



#endif // XMLPWRITER_H
