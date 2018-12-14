#include "xmlpwriter.h"

XMLPWriter::XMLPWriter( const MLP& mlp,const QStringList& inames, int ilaper, int xlaper)
    :mlp(mlp), inames(inames), ilAperture(ilaper), xlAperture(xlaper)
{
    xml.setAutoFormatting(true);
}


bool XMLPWriter::writeFile(QIODevice *device){

    Q_ASSERT( device);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE xmlp>");

    xml.writeStartElement("xmlp");
    xml.writeAttribute("version", "1.0");

    xml.writeStartElement("weights");

    for( size_t l=0; l<3; l++){                     // fix for now, 2 hidden + output
        auto w=mlp.weights(l);

        xml.writeStartElement("layer");
        xml.writeAttribute("number", QString::number(l));
        xml.writeAttribute("n", QString::number(w.rows()));
        xml.writeAttribute("m", QString::number(w.columns()));
        for( size_t i=0; i<w.rows(); i++){
            for( size_t j=0; j<w.columns(); j++){
                xml.writeStartElement("weight");
                xml.writeAttribute("i", QString::number(i));
                xml.writeAttribute("j", QString::number(j));
                xml.writeAttribute("value", QString::number(w(i,j)));
                xml.writeEndElement();
            }
        }

        // biases included in weights/ use bias neuron(1)

        xml.writeEndElement();  // layer
    }

    xml.writeEndElement();      // layers

    xml.writeStartElement("input-scaling");
    xml.writeAttribute("count", QString::number(mlp.inputs()));
    for( size_t j=0; j<mlp.inputs(); j++){
        xml.writeStartElement("range");
        xml.writeAttribute("j", QString::number(j));
        auto mm=mlp.scaling(j);
        xml.writeAttribute("min", QString::number(mm.first));
        xml.writeAttribute("max", QString::number(mm.second));
        xml.writeEndElement();
    }
    xml.writeEndElement();          // input-scaling

    xml.writeStartElement("output-scaling");
    xml.writeStartElement("range");
    auto mm=mlp.outputScaling();
    xml.writeAttribute("min", QString::number(mm.first));
    xml.writeAttribute("max", QString::number(mm.second));
    xml.writeEndElement();
    xml.writeEndElement();          // output-scaling

    xml.writeStartElement("input-names");
    xml.writeAttribute("count", QString::number(inames.size()));
    for( int j=0; j<inames.size(); j++){
        xml.writeStartElement("input");
        xml.writeAttribute("j", QString::number(j));
        xml.writeAttribute("name", inames[j]);
        xml.writeEndElement();
    }
    xml.writeEndElement();          // input-names

    xml.writeStartElement("aperture");
    xml.writeAttribute("iline", QString::number(ilAperture));
    xml.writeAttribute("xline", QString::number(xlAperture));
    xml.writeEndElement();  // aperture

    xml.writeEndElement();      // xmlp

    xml.writeEndDocument();

    return true;
}
