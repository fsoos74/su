#include "xnlwriter.h"

XNLWriter::XNLWriter( const NN& nn,
                      const QStringList& inames, const std::vector<std::pair<double,double>>& iranges,
                        const std::pair<double,double>& orange)
    :nn(nn), inames(inames), iranges(iranges), orange(orange)
{
    xml.setAutoFormatting(true);
}


bool XNLWriter::writeFile(QIODevice *device){

    Q_ASSERT( device);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE xnl>");

    xml.writeStartElement("xnl");
    xml.writeAttribute("version", "1.0");

    xml.writeStartElement("nn");

    xml.writeStartElement("sizes");
    xml.writeAttribute("nlayers", QString::number(nn.layer_count()));
    for( size_t l=0; l<nn.layer_count(); l++){
        xml.writeStartElement("size");
        xml.writeAttribute("layer", QString::number(l));
        xml.writeAttribute("size",QString::number(nn.layer_size(l)));
        xml.writeEndElement();      // size
    }
    xml.writeEndElement();          //sizes

    for( size_t l=0; l<nn.layer_count(); l++){

        xml.writeStartElement("layer");
        xml.writeAttribute("number", QString::number(l));

        xml.writeStartElement("weights");
        auto w=nn.weights(l);
        for( size_t i=0; i<w.rows(); i++){
            for( size_t j=0; j<w.columns(); j++){
                xml.writeStartElement("weight");
                xml.writeAttribute("i", QString::number(i));
                xml.writeAttribute("j", QString::number(j));
                xml.writeAttribute("value", QString::number(w(i,j)));
                xml.writeEndElement();
            }
        }
        xml.writeEndElement();  // weights

        xml.writeStartElement("biases");
        auto b=nn.biases(l);
        for( size_t j=0; j<w.columns(); j++){       // iases is one row, each column a neuron
            xml.writeStartElement("bias");
            xml.writeAttribute("j", QString::number(j));
            xml.writeAttribute("value", QString::number(b(0,j)));
            xml.writeEndElement();
        }
        xml.writeEndElement();  // biases

        xml.writeEndElement();  // layer
    }


    xml.writeEndElement();      // nn

    xml.writeStartElement("input-names");
    xml.writeAttribute("count", QString::number(inames.size()));
    for( int j=0; j<inames.size(); j++){
        xml.writeStartElement("input");
        xml.writeAttribute("j", QString::number(j));
        xml.writeAttribute("name", inames[j]);
        xml.writeEndElement();
    }
    xml.writeEndElement();          // input-names

    xml.writeStartElement("input-ranges");
    xml.writeAttribute("count", QString::number(iranges.size()));
    for( int j=0; j<iranges.size(); j++){
        xml.writeStartElement("range");
        xml.writeAttribute("j", QString::number(j));
        xml.writeAttribute("min", QString::number(iranges[j].first));
        xml.writeAttribute("max", QString::number(iranges[j].second));
        xml.writeEndElement();
    }
    xml.writeEndElement();          // input-ranges

    xml.writeStartElement("output-range");
    xml.writeAttribute("min", QString::number(orange.first));
    xml.writeAttribute("max", QString::number(orange.second));
    xml.writeEndElement();      // output range


    xml.writeEndElement();      // xnl

    xml.writeEndDocument();

    return true;
}
