#ifndef VIEWITEMMODEL_H
#define VIEWITEMMODEL_H

#include <QObject>
#include <QVector>

class ViewItem;

class ViewItemModel : public QObject
{
    Q_OBJECT
public:
    explicit ViewItemModel(QObject *parent = nullptr);
    int size()const{
        return mItems.size();
    }

    QStringList names();
    bool contains(QString);
    int indexOf(QString);
    ViewItem* at(int);

signals:
    void changed();
    void itemChanged(ViewItem*);

public slots:
    void clear();
    void add(ViewItem*);
    void remove(int);
    void swap(int,int);
    void moveUp(int);
    void moveDown(int);

private slots:
    void itemChanged();

private:
    QVector<ViewItem*> mItems;
};

#endif // VIEWITEMMODEL_H
