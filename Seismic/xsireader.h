#ifndef XSIREADER_H
#define XSIREADER_H


#include <QXmlStreamReader>
#include "segyinfo.h"

namespace seismic{

class XSIReader
{
public:

    XSIReader( SEGYInfo& info);

    bool read(QIODevice *device);

    QString errorString() const;

private:

    void readXSI();

    void readHeaderDef( std::vector<SEGYHeaderWordDef>& hd);

    SEGYInfo& m_info;

    QXmlStreamReader xml;


};

}

#endif // XSIREADER_H
