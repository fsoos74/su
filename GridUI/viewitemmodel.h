#ifndef VIEWITEMMODEL_H
#define VIEWITEMMODEL_H

#include <QObject>
#include <QVector>

namespace sliceviewer {

class ViewItem;

class ViewItemModel : public QObject
{
    Q_OBJECT
public:
    explicit ViewItemModel(QObject *parent = nullptr);
    int size()const{
        return mItems.size();
    }
    bool isMute()const{
        return mMute;
    }
    QStringList names();
    bool contains(QString);
    int indexOf(QString);
    ViewItem* at(int);

signals:
    void changed();
    void itemChanged(ViewItem*);

public slots:
    void setMute(bool);
    void clear();
    void add(ViewItem*);
    void remove(int);
    void swap(int,int);
    void moveUp(int);
    void moveDown(int);

protected:
    void fireChanged();
    void fireItemChanged(ViewItem*);

private slots:
    void itemChanged();

private:
    QVector<ViewItem*> mItems;
    bool mMute=false;
    int mChangedCounter=0;
};

}

#endif // VIEWITEMMODEL_H
