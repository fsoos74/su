#ifndef XPPREADER_H
#define XPPREADER_H

#include <QXmlStreamReader>

#include "processparams.h"


class XPPReader
{
public:

    XPPReader( ProcessParams& );

    bool readFile( QIODevice* );

    QString errorString()const;

private:

    QXmlStreamReader xml;
    ProcessParams& processParams;
};

#endif // XPPREADER_H
