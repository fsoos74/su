#include "xmlpreader.h"
#include "matrix.h"
#include<vector>

XMLPReader::XMLPReader(MLP& mlp, QStringList& inames)
    :mlp(mlp), inames(inames)
{

}



bool XMLPReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "xmlp" && xml.attributes().value("version") == "1.0"){

            readXMLP();
        }
        else{
            xml.raiseError(QObject::tr("The file is not an xmlp version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XMLPReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}


void XMLPReader::readXMLP(){

    while(xml.readNextStartElement()){

        std::cout<<"readXMLP name="<<xml.name().toString().toStdString()<<std::endl<<std::flush;
        if(xml.name()=="weights"){
            readWeights();
        }
        else if(xml.name()=="input-scaling"){
            readInputScaling();
        }
        else if( xml.name()=="input-names"){
            readINames();
        }
        else{
            xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }

    }
}

void XMLPReader::readWeights(){

    std::vector<Matrix<double>> W;

    while(xml.readNextStartElement()){
        std::cout<<"readWeights name="<<xml.name().toString().toStdString()<<std::endl<<std::flush;
        if( xml.name()=="layer"){
            size_t number=xml.attributes().value("number").toUInt();
            size_t n=xml.attributes().value("n").toUInt();
            size_t m=xml.attributes().value("m").toUInt();
            std::cout<<"READ WEIGHTS layer="<<number<<" n="<<n<<" m="<<m<<std::endl<<std::flush;
            auto Wi=readLayer(n,m);
            W.push_back(Wi);
            std::cout<<Wi<<std::endl<<std::endl<<std::flush;
        }
        else{
            xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
    }

    // now we can build mlp
    // only a simple lmlp is implemented yet: 2 hidden layers +1 output layer
    if( W.size()!=3){
        xml.raiseError("ONLY simple MLP!!!");
        return;
    }
    size_t nin=W[0].columns();
    size_t nh1=W[0].rows();
    size_t nh2=W[1].rows();
    size_t nout=W[2].rows();
    mlp=SimpleMLP(nin,nh1,nh2,nout);
    for( size_t i=0; i<W.size(); i++){
        mlp.setWeights(i,W[i]);
    }
    std::cout<<"FINISHED READING WEIGHTS"<<std::endl<<std::flush;
}

Matrix<double> XMLPReader::readLayer(size_t n, size_t m){

    Matrix<double> W(n,m);

    while( xml.readNextStartElement() ){
        if(xml.name()=="weight"){
            auto i = xml.attributes().value("i").toUInt();
            auto j = xml.attributes().value("j").toUInt();
            auto x = xml.attributes().value("value").toDouble();
            W(i,j)=x;
        }
        else{
            //std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
        xml.skipCurrentElement();
    }

    return W;
}

void XMLPReader::readINames(){

    std::cout<<"XMLPReader::readINames"<<std::endl<<std::flush;

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

    std::cout<<"/XMLPReader::readINames"<<std::endl<<std::flush;
}


void XMLPReader::readInputScaling(){

    std::cout<<"XMLPReader::readIRanges"<<std::endl<<std::flush;

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
            mlp.setScaling(j,std::make_pair(min,max));
        }
        else{
            std::cout<<"Skipping \""<<xml.name().toString().toStdString()<<"\""<<std::endl<<std::flush;
            //xml.skipCurrentElement();
            qWarning("Skipping unexpected element!");
        }
        xml.skipCurrentElement();
    }

    std::cout<<"/XMLPReader::readIRanges"<<std::endl<<std::flush;
}

