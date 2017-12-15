#ifndef XGRREADER_H
#define XGRREADER_H


#include <QXmlStreamReader>
#include "grid2d.h"

class XGRReader
{
public:

    XGRReader(Grid2D<float>* grid=nullptr);

    bool read(QIODevice *device);

    QString errorString() const;

    Grid2DBounds bounds()const{
        return m_bounds;
    }

private:

    void readXGR();

    void readPoint();

    QXmlStreamReader xml;
    Grid2DBounds m_bounds;
    Grid2D<float>* m_grid;
};



#endif // XGRREADER_H
