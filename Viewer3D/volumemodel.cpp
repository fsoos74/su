#include "volumemodel.h"

VolumeModel::VolumeModel(QObject *parent) : QObject(parent)
{

}


void VolumeModel::clear(){

    if( m_items.empty()) return;

    m_items.clear();

    emit changed();
}

void VolumeModel::addItem(QString name, VolumeDef slice){

    if( m_items.contains(name)) return;
    m_items.insert(name, slice);
    emit changed();
}

void VolumeModel::removeItem(QString name){

    if( ! m_items.contains(name)) return;

    m_items.remove(name);

    emit changed();
}

void VolumeModel::setItem(QString name, VolumeDef def){


    if( !m_items.contains(name)) return;

    if( m_items.value(name) == def ) return;  // no change

    m_items.insert(name, def);

    emit changed();
}
