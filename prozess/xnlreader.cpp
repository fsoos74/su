#include "xnlreader.h"

XNLReader::XNLReader(NN& nn, QStringList& inames, std::vector<std::pair<double,double>>& iranges, std::pair<double,double>& orange)
    :nn(nn), inames(inames), iranges(iranges), orange(orange)
{

}



bool XNLReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "xnl" && xml.attributes().value("version") == "1.0"){

            readXNL();
        }
        else{
            xml.raiseError(QObject::tr("The file is not an xnl version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XNLReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}


void XNLReader::readXNL(){

    std::cout<<"XNLReader::readXNL()"<<std::endl<<std::flush;
    while(xml.readNextStartElement()){

        if(xml.name()=="nn"){
            readNN();
        }
        else if( xml.name()=="input-names"){
            readINames();
        }
        else if( xml.name()=="input-ranges"){
            readIRanges();
        }
        else if( xml.name()=="output-range"){
            readORange();
        }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }

    }
}

void XNLReader::readNN(){

    std::cout<<"XNLReader::readNN()"<<std::endl<<std::flush;
    while(xml.readNextStartElement()){

        if(xml.name()=="sizes"){
            readSizes();
        }
        else if( xml.name()=="layer"){
            unsigned l=xml.attributes().value("number").toUInt();
            readLayer(l);
        }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }

    }

    std::cout<<"/XNLReader::readNN()"<<std::endl<<std::flush;
}

void XNLReader::readSizes(){

    std::cout<<"XNLReader::readSizes()"<<std::endl<<std::flush;

    unsigned nlayers=xml.attributes().value("nlayers").toUInt();

    std::vector<size_t> sizes(nlayers, 0);
    while( xml.readNextStartElement() ){
       if(xml.name()=="size"){
            auto l=xml.attributes().value("layer").toUInt();
            auto size=xml.attributes().value("size").toUInt();
            if(l>=nlayers){
                xml.raiseError("invalid layer number!");
                return;
            }
            sizes[l]=size;
       }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            //xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
        xml.skipCurrentElement();
    }

    nn=NN(sizes);

    std::cout<<"/XNLReader::readSizes()"<<std::endl<<std::flush;

}

void XNLReader::readLayer(unsigned l){

    std::cout<<"XNLReader::readLayer"<<std::endl<<std::flush;
   if( l>=nn.layer_count()){
        xml.raiseError("invalid layer number!");
        return;
    }
    while( xml.readNextStartElement() ){
       if(xml.name()=="weights"){
           readWeights(l);
       }
       else if(xml.name()=="biases"){
           readBiases(l);
       }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
        //xml.skipCurrentElement();
    }
    std::cout<<"/XNLReader::readLayer"<<std::endl<<std::flush;
}

void XNLReader::readWeights(unsigned l){

    std::cout<<"XNLReader::readWeights"<<std::endl<<std::flush;
    auto m = nn.weights(l);

    while( xml.readNextStartElement() ){
        if(xml.name()=="weight"){
            auto i = xml.attributes().value("i").toUInt();
            auto j = xml.attributes().value("j").toUInt();
            auto x = xml.attributes().value("value").toDouble();
            if( i>=m.rows() || j>=m.columns()){
                xml.raiseError("invalid indices");
                return;
            }
            m(i,j)=x;
        }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            //xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
        xml.skipCurrentElement();
    }

    nn.setWeights(l,m);

    std::cout<<"/XNLReader::readWeights"<<std::endl<<std::flush;
}



void XNLReader::readBiases(unsigned l){

    std::cout<<"XNLReader::readBiases"<<std::endl<<std::flush;
    auto m = nn.biases(l);

    while( xml.readNextStartElement() ){
        if(xml.name()=="bias"){
            auto j = xml.attributes().value("j").toUInt();
            auto x = xml.attributes().value("value").toDouble();
            if( j>=m.columns()){
                xml.raiseError("invalid index");
                return;
            }
            m(0,j)=x;
        }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            //xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
        xml.skipCurrentElement();
    }

    nn.setBiases(l,m);

    std::cout<<"/XNLReader::readBiases"<<std::endl<<std::flush;
}


void XNLReader::readINames(){

    std::cout<<"XNLReader::readINames"<<std::endl<<std::flush;

    unsigned ninputs=xml.attributes().value("count").toUInt();

    QVector<QString> tmp(ninputs);

    while( xml.readNextStartElement() ){
        if(xml.name()=="input"){
            auto j = xml.attributes().value("j").toUInt();
            auto name = xml.attributes().value("name").toString();
            if( j>=ninputs){
                xml.raiseError("invalid index");
                return;
            }
            tmp[j]=name;
        }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            //xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
        xml.skipCurrentElement();
    }

    inames=QStringList::fromVector(tmp);

    std::cout<<"/XNLReader::readINames"<<std::endl<<std::flush;
}


void XNLReader::readIRanges(){

    std::cout<<"XNLReader::readIRanges"<<std::endl<<std::flush;

    unsigned ninputs=xml.attributes().value("count").toUInt();

    std::vector<std::pair<double,double>> tmp(ninputs);

    while( xml.readNextStartElement() ){
        if(xml.name()=="range"){
            auto j = xml.attributes().value("j").toUInt();
            auto min = xml.attributes().value("min").toDouble();
            auto max = xml.attributes().value("max").toDouble();
            if( j>=ninputs){
                xml.raiseError("invalid index");
                return;
            }
            tmp[j]=std::make_pair(min,max);
        }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            //xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
        xml.skipCurrentElement();
    }

    iranges=tmp;

    std::cout<<"/XNLReader::readIRanges"<<std::endl<<std::flush;
}


void XNLReader::readORange(){

    std::cout<<"XNLReader::readORange"<<std::endl<<std::flush;

    auto min = xml.attributes().value("min").toDouble();
    auto max = xml.attributes().value("max").toDouble();
    orange=std::make_pair(min,max);

    xml.skipCurrentElement();


    std::cout<<"/XNLReader::readORange"<<std::endl<<std::flush;
}
