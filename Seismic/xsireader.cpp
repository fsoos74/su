#include "xsireader.h"

#include<iostream>

namespace seismic{



XSIReader::XSIReader( SEGYInfo& info) : m_info(info)
{

}


bool XSIReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {

        if (xml.name() == "SEGYInfo" && xml.attributes().value("version") == "1.0"){

            readXSI();
        }
        else{
            xml.raiseError(QObject::tr("The file is not an xsi version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XSIReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}


void XSIReader::readXSI(){


    m_info.setSwap(xml.attributes().value("swap")=="true");



    if( xml.attributes().value("format")=="header" ){
        m_info.setOverrideSampleFormat(false);
    }
    else{
        m_info.setSampleFormat(toSampleFormat(xml.attributes().value("format").toString()));
    }

    if( xml.attributes().value("scalco")=="header"){
        m_info.setFixedScalco(false);
    }
    else{
        m_info.setScalco(xml.attributes().value("scalco").toDouble());
    }
    //std::cout<<"XSI: scalco="<<xml.attributes().value("scalco").toString().toStdString()<<" m_info="<<m_info.scalco()<<std::endl<<std::flush;

    if( xml.attributes().value("scalel")=="header"){
        m_info.setFixedScalel(false);
    }
    else{
        m_info.setScalel(xml.attributes().value("scalel").toDouble());
    }
    //xml.skipCurrentElement();

    std::vector<SEGYHeaderWordDef> bhdef;
    std::vector<SEGYHeaderWordDef> thdef;

    xml.readNextStartElement();
    //std::cout<<"TAG <"<<xml.name().toString().toStdString()<<">"<<std::endl;
    if( xml.name()!="binary-header-def"){
        //std::cout<<"TAG <"<<xml.name().toString().toStdString()<<">"<<std::endl;
        xml.raiseError("Wrong tag, expected binary-header-def!");
        return;
    }
    readHeaderDef(bhdef);

    m_info.setBinaryHeaderDef(bhdef);

    xml.readNextStartElement();
    if( xml.name()!="trace-header-def"){
        xml.raiseError("Wrong tag, expected trace-header-def!");
        return;
    }
    readHeaderDef(thdef);


    m_info.setTraceHeaderDef(thdef);

}


void XSIReader::readHeaderDef( std::vector<SEGYHeaderWordDef>& hd){

    while( xml.readNextStartElement()){

        if( xml.name()=="word"){


            SEGYHeaderWordDef def(
                xml.attributes().value("name").toString().toStdString(),
                toConvType(xml.attributes().value("ctype").toString()),
                toDataType(xml.attributes().value("dtype").toString()),
                xml.attributes().value("pos").toUInt(),
                xml.attributes().value("descr").toString().toStdString());

            hd.push_back(def);

            //std::cout<<"rhd: "<<def.name<<" "<<def.pos<<std::endl;

        }
        else{
            qWarning("Skipping unexpected element!");
        }
        xml.skipCurrentElement();
    }

}



}
