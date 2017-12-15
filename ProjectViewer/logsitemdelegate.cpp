#include "logsitemdelegate.h"

#include <QStyledItemDelegate>
#include <QComboBox>

#include<iostream>

LogsItemDelegate::LogsItemDelegate(QObject* parent): QStyledItemDelegate(parent){
}

LogsItemDelegate::~LogsItemDelegate(){
}

QWidget* LogsItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const{

    std::cout<<"LogsItemDelegate::createEditor row="<<index.row()<<" col="<<index.column()<<std::endl<<std::flush;
    // ComboBox ony in column 5
    if (index.column() != 5){
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    // Create the combobox and populate it
    QComboBox* cb = new QComboBox(parent);
    QVariant v=index.data(Qt::UserRole+1);
    cb->addItems( v.toStringList() );
    return cb;
}

void LogsItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor)) {
       // get the index of the text in the combobox that matches the current value of the itenm
       QString currentText = index.data(Qt::EditRole).toString();
       int cbIndex = cb->findText(currentText);
       // if it is valid, adjust the combobox
       if (cbIndex >= 0)
           cb->setCurrentIndex(cbIndex);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void LogsItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const{
    QComboBox* cb= dynamic_cast<QComboBox*>(editor);
    if( !cb) return;
    return; // keep data as is
}
