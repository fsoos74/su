#include "baseviewer.h"

#include<iostream>

BaseViewer::BaseViewer(QWidget *parent) : QMainWindow(parent)
{

}

BaseViewer::~BaseViewer(){

    if( m_dispatcher){
        m_dispatcher->removeViewer(this);
    }
}

void BaseViewer::setDispatcher(std::shared_ptr<PointDispatcher> pd){

    // disconnect from old if exists
    if( m_dispatcher){
        m_dispatcher->removeViewer(this);
    }

    m_dispatcher=pd;

    // connect to dispatcher if valid
    if( m_dispatcher ){

        m_dispatcher->addViewer(this);
    }
}

void BaseViewer::setReceptionEnabled(bool on){

    if( on==m_receptionEnabled) return;

    m_receptionEnabled=on;

    emit receptionEnabledChanged(on);
}

void BaseViewer::setBroadcastEnabled(bool on){

    if( on==m_broadcastEnabled) return;

    m_broadcastEnabled=on;

    emit broadcastEnabledChanged(on);
}

void BaseViewer::sendPoint(SelectionPoint point, int code){

    if( !m_broadcastEnabled) return;

    if( m_dispatcher){
        m_dispatcher->sendPoint(point, code, this);
    }
}

void BaseViewer::sendPoints(QVector<SelectionPoint> points, int code){

    if( !m_broadcastEnabled) return;

    if( m_dispatcher){
        m_dispatcher->sendPoints(points, code, this);
    }
}

