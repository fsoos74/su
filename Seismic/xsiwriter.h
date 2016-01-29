#ifndef XSIWRITER_H
#define XSIWRITER_H



#include<QXmlStreamWriter>
#include "segyinfo.h"

namespace seismic{



class XSIWriter
{
public:

    XSIWriter(const SEGYInfo&);

    bool writeFile(QIODevice *device);

private:

    void writeHeaderWordDef( const seismic::SEGYHeaderWordDef&);

    const SEGYInfo& m_info;
    QXmlStreamWriter xml;
};


}

#endif // XSIWRITER_H
