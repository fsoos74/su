#ifndef XGRREADER_H
#define XGRREADER_H


#include <QXmlStreamReader>
#include "grid2d.h"

class XGRReader
{
public:

    XGRReader(Grid2D<float>& grid);

    bool read(QIODevice *device);

    QString errorString() const;

private:

    void readXGR();

    void readPoint();

    QXmlStreamReader xml;
    Grid2D<float>& m_grid;
};



#endif // XGRREADER_H
