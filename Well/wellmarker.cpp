#include "wellmarker.h"

WellMarker::WellMarker(QString name, QString uwi, qreal md):m_name(name), m_uwi(uwi), m_md(md)
{

}


void WellMarker::setName(QString s){
    m_name=s;
}


void WellMarker::setMD(qreal d){
    m_md=d;
}
