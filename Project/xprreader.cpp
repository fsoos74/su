#include "xprreader.h"

#include "axxisorientation.h"
#include<iostream>
#include<QDir>


XPRReader::XPRReader( AVOProject* project, QString projectFilePath)
    : m_project( project ), m_projectFilePath(projectFilePath)
{
    Q_ASSERT( project );
}

bool XPRReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if(xml.readNextStartElement()) {

        if (xml.name() == "xpr" && xml.attributes().value("version") == "1.0"){

            xml.readNextStartElement();
            if( xml.name()!="project-directory"){
                xml.raiseError("Unexpected element");
            }
            else{
                QString projectDirectory=xml.readElementText();

                if( QDir(projectDirectory).isRelative()  ){
                    // make relative (to xpr file) project directory absolute
                    QString pdir=QFileInfo(m_projectFilePath).path()+QDir::separator()+projectDirectory;
                    projectDirectory=QDir(pdir).canonicalPath();
                    //std::cout<<"project dir: "<<projectDirectory.toStdString()<<std::endl<<std::flush;
                }

                try{  // ADD: setProjectDirectory should remember old value and restore it upon failure!!!
                    m_project->setProjectDirectory(projectDirectory);
                }catch(AVOProject::PathException& ex){
                    xml.raiseError(QString("Creating project failed: %1").arg(ex.what()));
                    return false;
                }

            }

           readGeometry();

           readOrientation();
        }
        else{
            xml.raiseError(QObject::tr("The file is not an xpr version 1.0 file."));
        }
    }

    return !xml.error();
}


// store geom in m_project, xml will be set error if failed
// for compability it is no erro if no geometry is found
void XPRReader::readGeometry(){

    // read geometry if exists, check is done because of compatabiity with old version
    if(xml.readNextStartElement()) {

        if (xml.name() == "project-geometry" ){

            ProjectGeometry geom;

            for( int i=0; i<3; i++){

                QString key=QString("inline%1").arg(i+1);
                if(!xml.readNextStartElement() || xml.name()!=key ){
                    xml.raiseError("Unexpected element!");
                    break;
                }
                int il=xml.readElementText().toInt();

                key=QString("crossline%1").arg(i+1);
                if(!xml.readNextStartElement() || xml.name()!=key ){
                    xml.raiseError("Unexpected element!");
                    break;
                }
                int xl=xml.readElementText().toInt();

                key=QString("x%1").arg(i+1);
                if(!xml.readNextStartElement() || xml.name()!=key ){
                    xml.raiseError("Unexpected element!");
                    break;
                }
                double x=xml.readElementText().toDouble();

                key=QString("y%1").arg(i+1);
                if(!xml.readNextStartElement() || xml.name()!=key ){
                    xml.raiseError("Unexpected element!");
                    break;
                }
                double y=xml.readElementText().toDouble();

                geom.lines( i ) = QPoint(il,xl);
                geom.coords( i ) = QPointF(x,y);
            }

            xml.skipCurrentElement();
            //xml.readNext();

            m_project->setGeometry(geom);
        }
        else{
            xml.raiseError("Unexpected element");
        }
    }else{
        std::cout<<"No Geometry"<<std::endl;
    }

}

void XPRReader::readOrientation(){

    // read orientation if exists, check is done because of compatabiity with old version
    if(xml.readNextStartElement()) {

        if (xml.name() == "axis-setup" ){

            if(!xml.readNextStartElement() || xml.name()!="inline-orientation" ){
                xml.raiseError("Unexpected element!");
            }
            AxxisOrientation inline_orientation=toAxxisOrientation( xml.readElementText() );

            if(!xml.readNextStartElement() || xml.name()!="inline-direction" ){
                xml.raiseError("Unexpected element!");
            }
            AxxisDirection inline_direction=toAxxisDirection( xml.readElementText() );

            if(!xml.readNextStartElement() || xml.name()!="crossline-direction" ){
                xml.raiseError("Unexpected element!");
            }
            AxxisDirection crossline_direction=toAxxisDirection( xml.readElementText() );

            m_project->setInlineOrientation(inline_orientation );
            m_project->setInlineDirection(inline_direction);
            m_project->setCrosslineDirection(crossline_direction);

       }
   }
}

QString XPRReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}
