#ifndef HORIZONMODEL_H
#define HORIZONMODEL_H


#include <QObject>
#include <QMap>
#include <horizondef.h>

/*
 * This is basically the same as SliceModel.
 * Qt doesn't support templates for classed derived from QObject
 * That is why I had to copy SliceModel
 * It should have been Model<SliceDef>, Model<HorizonDef>
*/

class HorizonModel : public QObject
{
    Q_OBJECT
public:
    explicit HorizonModel(QObject *parent = 0);

    int size()const{
        return m_items.size();
    }

    QList<QString> names()const{
        return m_items.keys();
    }

    bool contains(QString name){
        return m_items.contains(name);
    }

    HorizonDef item(QString name){
        return m_items.value(name);
    }

signals:
    void changed();

public slots:

    void clear();
    void addItem( QString, HorizonDef );
    void removeItem( QString );
    void setItem( QString, HorizonDef );

private:

    QMap<QString, HorizonDef > m_items;
};


#endif // HORIZONMODEL_H
