#ifndef XPRWRITER_H
#define XPRWRITER_H


#include<QXmlStreamWriter>

#include"avoproject.h"


class XPRWriter
{
public:

    XPRWriter( const AVOProject& project);

    bool writeFile(QIODevice *device);

private:

    const AVOProject& m_project;
    QXmlStreamWriter xml;
};


#endif // XPRWRITER_H
