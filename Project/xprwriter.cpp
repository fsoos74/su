#include "xprwriter.h"



XPRWriter::XPRWriter( const AVOProject& project):m_project(project){
    xml.setAutoFormatting(true);
}

bool XPRWriter::writeFile(QIODevice *device){
    Q_ASSERT( device);

    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE avoproject>");

    xml.writeStartElement("xpr");
    xml.writeAttribute("version", "1.0");

    xml.writeTextElement("project-directory", m_project.projectDirectory());

    const ProjectGeometry& geom=m_project.geometry();

    xml.writeStartElement("project-geometry");
    for( int i=0; i<3; i++){

        xml.writeTextElement(QString("inline%1").arg(i+1), QString::number(geom.inlineAndCrossline(i).x() ) );
        xml.writeTextElement(QString("crossline%1").arg(i+1), QString::number( geom.inlineAndCrossline(i).y() ) );
        xml.writeTextElement(QString("x%1").arg(i+1),QString::number( geom.coordinates(i).x() ) );
        xml.writeTextElement(QString("y%1").arg(i+1), QString::number( geom.coordinates(i).y() ) );
    }
    xml.writeEndElement();


     xml.writeEndElement();


    xml.writeEndDocument();

    return !(xml.hasError());
}
