#include "volumeitemmodel.h"
#include "volumeitem.h"
#include<iostream>

VolumeItemModel::VolumeItemModel(QObject *parent) : QObject(parent)
{

}

QStringList VolumeItemModel::names(){
    QStringList list;
    for(auto vitem : mItems){
        list.append(vitem->name());
    }
    return list;
}

int VolumeItemModel::indexOf(QString name){
    for(int i=0; i<mItems.size(); i++){
        auto vitem=mItems[i];
        if(vitem->name()==name) return i;
    }
    return -1;  // nothing found
}

bool VolumeItemModel::contains(QString name){
    return (indexOf(name)>=0);
 }

VolumeItem* VolumeItemModel::at(int i){
    return mItems.at(i);
}

void VolumeItemModel::clear(){
    mItems.clear();
    emit changed();
}

void VolumeItemModel::add(VolumeItem * vitem){
    if(!vitem) return;
    mItems.append(vitem);
    vitem->setParent(this);
    connect(vitem, SIGNAL(changed()), this, SLOT(itemChanged()));
    emit changed();
}

void VolumeItemModel::remove(int i){
    if(i<0 || i>= mItems.size() ) return;
    mItems.remove(i);
    emit changed();
}

void VolumeItemModel::swap(int i1, int i2){
    if(i1<0 || i1>=mItems.size() || i2<0 || i2>=mItems.size() || i1==i2) return;
    auto temp=mItems.at(i1);
    mItems.replace(i1, mItems.at(i2));
    mItems.replace(i2, temp);
    std::cout<<"swap "<<i1<<" "<<i2<<std::endl<<std::flush;
    emit changed();
}

void VolumeItemModel::moveUp(int i){
    std::cout<<"VolumeItemModel::moveUp i="<<i<<std::endl<<std::flush;
    if(i<1 || i>=mItems.size()) return;
    swap(i,i-1);
}

void VolumeItemModel::moveDown(int i){
     std::cout<<"VolumeItemModel::moveDown i="<<i<<std::endl<<std::flush;
    if(i<0 || i+1>=mItems.size()) return;
    swap(i,i+1);
}

void VolumeItemModel::itemChanged(){
    VolumeItem* vitem=dynamic_cast<VolumeItem*>(QObject::sender());
    if(vitem) emit itemChanged(vitem);
}
