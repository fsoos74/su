#include "horizonmanager.h"


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

    if( ! m_horizonDefs.contains(name) ) return;

    if( m_horizonDefs.value(name) == def ) return;

    m_horizonDefs.insert( name, def);   // insert replaces existing values

    emit paramsChanged(name);
}
