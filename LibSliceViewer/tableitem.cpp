#include "tableitem.h"

namespace sliceviewer {


TableItem::TableItem(QObject *parent) : ViewItem(parent), mColor(Qt::blue), mPointSize(2), mOpacity(100)
{

}

void TableItem::setTable(std::shared_ptr<Table> table){
    if(table==mTable) return;
    mTable=table;
    emit changed();
}

void TableItem::setColor(QColor color){
    if(color==mColor) return;
    mColor=color;
    emit changed();
}

void TableItem::setPointSize(int pointSize){
    if(pointSize==mPointSize) return;
    mPointSize=pointSize;
    emit changed();
}

void TableItem::setOpacity(int opacity){
    if(opacity==mOpacity) return;
    mOpacity=opacity;
    emit changed();
}

}
