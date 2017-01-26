#ifndef HORIZONMANAGER_H
#define HORIZONMANAGER_H

#include <QObject>
#include <QMap>
#include <horizondef.h>
#include <grid2d.h>

#include <memory>

class HorizonManager : public QObject
{
    Q_OBJECT
public:
    explicit HorizonManager(QObject *parent = 0);

    QList<QString> names(){
        return m_horizons.keys();
    }

    void clear(){
        m_horizons.clear();
        m_horizonDefs.clear();
    }

    int size(){
        return m_horizons.size();
    }

    bool contains( QString name ){
        return m_horizons.contains(name);
    }

    HorizonDef params(QString name){
        return m_horizonDefs.value(name);
    }

    Grid2D<float>* horizon(QString name){
        return m_horizons.value(name).get();
    }

signals:

    void horizonsChanged();
    void paramsChanged( QString );

public slots:

    void add( QString, HorizonDef, std::shared_ptr<Grid2D<float>>);
    void remove( QString );
    void setParams( QString, HorizonDef );

private:

    QMap<QString, HorizonDef> m_horizonDefs;
    QMap<QString, std::shared_ptr<Grid2D<float> > >  m_horizons;
};

#endif // HORIZONMANAGER_H
