#include "horizonmanager.h"

#include<iostream>


HorizonManager::HorizonManager(QObject *parent) : QObject(parent)
{

}


void HorizonManager::add( QString name, HorizonDef def, std::shared_ptr<Grid2D<float>> h ){

    if( m_horizons.contains(name) ) return; // no dupes

    m_horizons.insert( name, h );
    m_horizonDefs.insert(name, def);

    emit horizonsChanged();
}

void HorizonManager::remove( QString name){

    if( ! m_horizons.contains(name) ) return;

    m_horizons.remove(name);
    m_horizonDefs.remove(name);

    emit horizonsChanged();
}

void HorizonManager::setParams( QString name, HorizonDef def ){

    std::cout<<"HorizonManager::setParams name="<<name.toStdString()<<std::endl;
    if( ! m_horizonDefs.contains(name) ) return;

    std::cout<<"HorizonManager::setParams 2"<<std::endl;
    if( m_horizonDefs.value(name) == def ) return;

    std::cout<<"HorizonManager::setParams 3"<<std::endl;
    m_horizonDefs.insert( name, def);   // insert replaces existing values

    std::cout<<"HorizonManager::setParams 4"<<std::endl;
    emit paramsChanged(name);
}
