#include "viewitemmodel.h"
#include "viewitem.h"

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

void ViewItemModel::clear(){
    mItems.clear();
    emit changed();
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
            emit changed();
            return;
        }
    }
    // no unique name found, nothing inserted
}

void ViewItemModel::remove(int i){
    if(i<0 || i>= mItems.size() ) return;
    mItems.remove(i);
    emit changed();
}

void ViewItemModel::swap(int i1, int i2){
    if(i1<0 || i1>=mItems.size() || i2<0 || i2>=mItems.size() || i1==i2) return;
    auto temp=mItems.at(i1);
    mItems.replace(i1, mItems.at(i2));
    mItems.replace(i2, temp);
    emit changed();
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
    if(vitem) emit itemChanged(vitem);
}
