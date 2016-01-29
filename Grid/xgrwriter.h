#ifndef XGRWriter_H
#define XGRWriter_H

#include<QXmlStreamWriter>
#include "grid2d.h"


//class Grid2D;

class XGRWriter
{
public:

    XGRWriter( const Grid2D<double>&);

    bool writeFile(QIODevice *device);

private:



    const Grid2D<double>& m_grid;
    QXmlStreamWriter xml;
};

#endif // XGRWriter_H
