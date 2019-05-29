#include "viewitemmodel.h"
#include "viewitem.h"

namespace sliceviewer {

ViewItemModel::ViewItemModel(QObject *parent) : QObject(parent)
{

}

QStringList ViewItemModel::names(){
    QStringList list;
    for(auto vitem : mItems){
        list.append(vitem->name());
    }
    return list;
}

int ViewItemModel::indexOf(QString name){
    for(int i=0; i<mItems.size(); i++){
        auto vitem=mItems[i];
        if(vitem->name()==name) return i;
    }
    return -1;  // nothing found
}

bool ViewItemModel::contains(QString name){
    return (indexOf(name)>=0);
 }

ViewItem* ViewItemModel::at(int i){
    return mItems.at(i);
}

void ViewItemModel::setMute(bool mute){
    if(mute==mMute) return;
    mMute=mute;
    if(!mute){   // unmuted, catch up with changes
        if(mChangedCounter>0){
            emit changed();
        }
        // only notity about most recent change, maybe better use loop and propagete all changes?
        if(mChangedItems.size()>0){
            emit itemChanged(mChangedItems.last());
        }
    }
    mChangedCounter=0;
    mChangedItems.clear();
}

void ViewItemModel::fireChanged(){
    if( !mMute){
        emit changed();
    }
    else{
        mChangedCounter++;
    }
}

void ViewItemModel::fireItemChanged(ViewItem * item){ 
    if( !mMute){
        emit itemChanged(item);
    }
    else{
        mChangedItems.append(item);
    }
}

void ViewItemModel::clear(){
    mItems.clear();
    fireChanged();
}

// this will also adjust the name to be unique within model, multiples will has (2), (3), ... added to name
void ViewItemModel::add(ViewItem * vitem){
    if(!vitem) return;
    // make name unique within mode
    for(int i=1; i<999; i++){
        QString name=vitem->name();
        if(i>1) name+=QString("(%1)").arg(i);
        if(!contains(name)){        // found unique name
            vitem->setName(name);   // update item with new name
            // now insert item
            mItems.append(vitem);
            vitem->setParent(this);
            connect(vitem, SIGNAL(changed()), this, SLOT(itemChanged()));
            fireChanged();
            return;
        }
    }
    // no unique name found, nothing inserted
}

void ViewItemModel::remove(int i){
    if(i<0 || i>= mItems.size() ) return;
    mItems.remove(i);
    fireChanged();
}

void ViewItemModel::swap(int i1, int i2){
    if(i1<0 || i1>=mItems.size() || i2<0 || i2>=mItems.size() || i1==i2) return;
    auto temp=mItems.at(i1);
    mItems.replace(i1, mItems.at(i2));
    mItems.replace(i2, temp);
    fireChanged();
}

void ViewItemModel::moveUp(int i){
    if(i<1 || i>=mItems.size()) return;
    swap(i,i-1);
}

void ViewItemModel::moveDown(int i){
    if(i<0 || i+1>=mItems.size()) return;
    swap(i,i+1);
}

void ViewItemModel::itemChanged(){
    ViewItem* vitem=dynamic_cast<ViewItem*>(QObject::sender());
    if(vitem) fireItemChanged(vitem);
}

}
