#include "horizonmanager.h"


HorizonManager::HorizonManager(QObject *parent) : QObject(parent)
{

}


void HorizonManager::add( QString name, HorizonParameters def, std::shared_ptr<Grid2D<float>> h ){

    if( m_horizons.contains(name) ) return; // no dupes

    m_horizons.insert( name, h );
    m_HorizonParameterss.insert(name, def);

    emit horizonsChanged();
}

void HorizonManager::remove( QString name){

    if( ! m_horizons.contains(name) ) return;

    m_horizons.remove(name);
    m_HorizonParameterss.remove(name);

    emit horizonsChanged();
}

void HorizonManager::setParams( QString name, HorizonParameters def ){

    if( ! m_HorizonParameterss.contains(name) ) return;

    if( m_HorizonParameterss.value(name) == def ) return;

    m_HorizonParameterss.insert( name, def);   // insert replaces existing values

    emit paramsChanged(name);
}
