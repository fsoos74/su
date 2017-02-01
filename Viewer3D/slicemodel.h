#ifndef SLICEMODEL_H
#define SLICEMODEL_H

#include <QObject>
#include <QMap>
#include <slicedef.h>

class SliceModel : public QObject
{
    Q_OBJECT
public:
    explicit SliceModel(QObject *parent = 0);

    int size()const{
        return m_slices.size();
    }

    QList<QString> names()const{
        return m_slices.keys();
    }

    bool contains(QString name){
        return m_slices.contains(name);
    }

    SliceDef slice(QString name){
        return m_slices.value(name);
    }


    QString generateName();

signals:
    void changed();

public slots:

    void clear();
    void addSlice( QString, SliceDef );
    void removeSlice( QString );
    void setSlice( QString, SliceDef );

private:

    QMap<QString, SliceDef > m_slices;
};

#endif // SLICEMODEL_H
