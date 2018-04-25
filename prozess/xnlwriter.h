#ifndef XNLWRITER_H
#define XNLWRITER_H

#include<QXmlStreamWriter>
#include<QStringList>
#include "nn.h"


class XNLWriter
{
public:

    XNLWriter(const NN& nn,
              const QStringList& inames, const std::vector<std::pair<double,double>>& ranges,
              const std::pair<double,double>& orange);

    bool writeFile(QIODevice *device);

private:

    const NN& nn;
    const QStringList& inames;
    const std::vector<std::pair<double,double>> iranges;
    const std::pair<double,double>& orange;
    QXmlStreamWriter xml;
};


#endif // XNLWRITER_H
