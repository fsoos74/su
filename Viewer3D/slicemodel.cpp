#include "slicemodel.h"

SliceModel::SliceModel(QObject *parent) : QObject(parent)
{

}


QString SliceModel::generateName(){

    const int maxID=999;

    for( int id=1; id<maxID; id++ ){
        QString name=QString("Slice #%1").arg(id);
        if( !m_slices.contains(name)) return name;
    }

    qWarning("IDs exhausted in SliceModel::generateName");

    return QString();
}

void SliceModel::clear(){

    if( m_slices.empty()) return;

    m_slices.clear();

    emit changed();
}


void SliceModel::addSlice(QString name, SliceDef slice){

    if( m_slices.contains(name)) return;

    m_slices.insert(name, slice);

    emit changed();
}

void SliceModel::removeSlice(QString name){

    if( ! m_slices.contains(name)) return;

    m_slices.remove(name);

    emit changed();
}

void SliceModel::setSlice(QString name, SliceDef slice){

    if( !m_slices.contains(name)) return;

    if( m_slices.value(name) == slice ) return;  // no change

    m_slices.insert(name, slice);

    emit changed();
}
