#ifndef XMLPREADER_H
#define XMLPREADER_H

#include <QXmlStreamReader>
#include "simplemlp.h"
#include "matrix.h"

class XMLPReader
{
public:

    using MLP=SimpleMLP;

    XMLPReader(MLP& nn, QStringList& inames, int& ilAperture, int& xlAperture);

    bool read(QIODevice *device);

    QString errorString() const;

private:

    void readXMLP();
    void readWeights();
    Matrix<double> readLayer(size_t n, size_t m);
    void readINames( );
    void readAperture();
    void readInputScaling( );
    void readOutputScaling( );

    QXmlStreamReader xml;
    MLP& mlp;
    QStringList& inames;
    int& ilAperture;
    int& xlAperture;
};

#endif // XMLPREADER_H
