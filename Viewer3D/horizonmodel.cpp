#include "horizonmodel.h"

HorizonModel::HorizonModel(QObject *parent) : QObject(parent)
{

}


void HorizonModel::clear(){

    if( m_items.empty()) return;

    m_items.clear();

    emit changed();
}
#include<iostream>

void HorizonModel::addItem(QString name, HorizonDef slice){

    if( m_items.contains(name)) return;
    m_items.insert(name, slice);
    emit changed();
}

void HorizonModel::removeItem(QString name){

    if( ! m_items.contains(name)) return;

    m_items.remove(name);

    emit changed();
}

void HorizonModel::setItem(QString name, HorizonDef def){


    if( !m_items.contains(name)) return;

    if( m_items.value(name) == def ) return;  // no change

    m_items.insert(name, def);

    emit changed();
}
