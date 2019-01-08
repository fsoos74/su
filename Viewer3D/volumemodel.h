#ifndef VOLUMEMODEL_H
#define VOLUMEMODEL_H


#include <QObject>
#include <QMap>
#include <volumedef.h>

/*
 * This is basically the same as SliceModel.
 * Qt doesn't support templates for classed derived from QObject
 * That is why I had to copy SliceModel
 * It should have been Model<SliceDef>, Model<HorizonDef>, Model<VolumeDef>
*/

class VolumeModel : public QObject
{
    Q_OBJECT
public:
    explicit VolumeModel(QObject *parent = 0);

    int size()const{
        return m_items.size();
    }

    QList<QString> names()const{
        return m_items.keys();
    }

    bool contains(QString name){
        return m_items.contains(name);
    }

    VolumeDef item(QString name){
        return m_items.value(name);
    }

signals:
    void changed();

public slots:

    void clear();
    void addItem( QString, VolumeDef );
    void removeItem( QString );
    void setItem( QString, VolumeDef );

private:

    QMap<QString, VolumeDef > m_items;
};


#endif // VOLUMEMODEL_H
