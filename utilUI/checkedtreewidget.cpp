#include "checkedtreewidget.h"
#include<QStandardItem>

CheckedTreeWidget::CheckedTreeWidget(QWidget* parent):
    QTreeWidget(parent)
{
    connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onItemClicked(QTreeWidgetItem*,int)));
}

QTreeWidgetItem* CheckedTreeWidget::findItem(QString name){
    for( int i=0; i<QTreeWidget::topLevelItemCount(); i++){
        auto item=QTreeWidget::topLevelItem(i);
        auto iname=item->text(0);
        if( iname==name) return item;
    }
    return nullptr;
}

QTreeWidgetItem* CheckedTreeWidget::findItem(QTreeWidgetItem* topItem, QString name){
    if( !topItem) return nullptr;
    for( int i=0; i<topItem->childCount(); i++){
        auto item=topItem->child(i);
        auto iname=item->text(0);
        if( iname==name) return item;
    }
    return nullptr;
}

QTreeWidgetItem* CheckedTreeWidget::findItem(QString top, QString name){
    auto topItem=findItem(top);
    auto item=findItem(topItem,name);
    return item;
}

bool CheckedTreeWidget::containsItem(QString name){
    return findItem(name)!=nullptr;
}

bool CheckedTreeWidget::containsItem(QString top, QString name){
    return findItem(top,name)!=nullptr;
}

QStringList CheckedTreeWidget::items(){
    QStringList list;
    for( int i=0; i<QTreeWidget::topLevelItemCount(); i++){
        auto item=QTreeWidget::topLevelItem(i);
        auto iname=item->text(0);
        list.append(iname);
    }
    return list;
}

QStringList CheckedTreeWidget::items(QString topName){
    QStringList list;
    auto topItem=findItem(topName);
    if(!topItem) return list;
    for( int i=0; i<topItem->childCount(); i++){
        auto item=topItem->child(i);
        auto iname=item->text(0);
        list.append(iname);
    }
    return list;
}

bool CheckedTreeWidget::isChecked(QString top, QString name){
    auto item=findItem(top,name);
    if( item==nullptr) return false;
    return item->checkState(0)==Qt::Checked;
}

void CheckedTreeWidget::addItem(QString name,bool checkable){
    if(findItem(name)) return;	// no dupes
    auto item=new QTreeWidgetItem();
    item->setText(0,name);
    if(checkable){
        item->setCheckState(0, Qt::Unchecked);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    }
    QTreeWidget::addTopLevelItem(item);
}

void CheckedTreeWidget::addItem(QString topName, QString name, bool checked){
    auto top=findItem(topName);
    if(top==nullptr) return;
    if(findItem(top,name)) return;	// no dupes
    auto item=new QTreeWidgetItem(top);
    item->setText(0,name);
    item->setCheckState(0, (checked)?Qt::Checked:Qt::Unchecked);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
}

void CheckedTreeWidget::setChecked(QString top, QString name, bool checked){
    auto item=findItem(top,name);
    if(item==nullptr) return;
    item->setCheckState(0, (checked)?Qt::Checked:Qt::Unchecked);
}

// check/uncheck all child according to parent
void CheckedTreeWidget::onItemClicked(QTreeWidgetItem* item, int col){
    if(!item) return;
    if(!item->flags()&Qt::ItemIsUserCheckable) return;
    auto checked=item->checkState(0);
    for ( int i=0; i<item->childCount(); i++){
        auto citem=item->child(i);
        if(citem->flags()&Qt::ItemIsUserCheckable){
            citem->setCheckState(0,checked);
        }
    }
}
