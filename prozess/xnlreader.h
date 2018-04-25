#ifndef XNLREADER_H
#define XNLREADER_H


#include <QXmlStreamReader>
#include "nn.h"

class XNLReader
{
public:

    XNLReader(NN& nn, QStringList& inames, std::vector<std::pair<double,double>>& iranges, std::pair<double,double>& orange);

    bool read(QIODevice *device);

    QString errorString() const;

private:

    void readXNL();

    void readNN();

    void readSizes();

    void readLayer(unsigned l);

    void readWeights( unsigned l);

    void readBiases( unsigned l);

    void readINames( );

    void readIRanges( );

    void readORange();

    QXmlStreamReader xml;
    NN& nn;
    QStringList& inames;
    std::vector<std::pair<double,double>>& iranges;
    std::pair<double,double>& orange;

};

#endif // XNLREADER_H
