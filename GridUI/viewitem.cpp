#include "viewitem.h"

ViewItem::ViewItem(QObject *parent) : QObject(parent)
{

}

void ViewItem::setName(QString name){
    if(name==mName) return;
    mName=name;
    emit changed();
}
