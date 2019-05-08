#ifndef VOLUMEITEMMODEL_H
#define VOLUMEITEMMODEL_H

#include <QObject>
#include <QVector>

class VolumeItem;

class VolumeItemModel : public QObject
{
    Q_OBJECT
public:
    explicit VolumeItemModel(QObject *parent = nullptr);
    int size()const{
        return mItems.size();
    }

    QStringList names();
    bool contains(QString);
    int indexOf(QString);
    VolumeItem* at(int);

signals:
    void changed();
    void itemChanged(VolumeItem*);

public slots:
    void clear();
    void add(VolumeItem*);
    void remove(int);
    void swap(int,int);
    void moveUp(int);
    void moveDown(int);

private slots:
    void itemChanged();

private:
    QVector<VolumeItem*> mItems;
};

#endif // VOLUMEITEMMODEL_H
