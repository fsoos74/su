#ifndef TABLEITEM_H
#define TABLEITEM_H

#include "viewitem.h"
#include <memory>
#include <table.h>
#include <QColor>

class TableItem : public ViewItem
{
    Q_OBJECT
public:
    explicit TableItem(QObject *parent = nullptr);
    std::shared_ptr<Table> table()const{
        return mTable;
    }
    QColor color()const{
        return mColor;
    }
    int pointSize()const{
        return mPointSize;
    }
    int opacity()const{
        return mOpacity;
    }

public slots:
    void setTable(std::shared_ptr<Table>);
    void setColor(QColor);
    void setPointSize(int);
    void setOpacity(int);

private:
    std::shared_ptr<Table> mTable;
    QColor mColor;
    int mPointSize;
    int mOpacity;
};

#endif // TABLEITEM_H
