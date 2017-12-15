#ifndef LOGSITEMDELEGATE_H
#define LOGSITEMDELEGATE_H

#include <QStyledItemDelegate>

class LogsItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    LogsItemDelegate(QObject* parent=0);
    ~LogsItemDelegate();

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

};

#endif // LOGSITEMDELEGATE_H
