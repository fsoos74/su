#ifndef HORIZONMANAGER_H
#define HORIZONMANAGER_H

#include <QObject>
#include <QMap>
#include <horizonparameters.h>
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
        m_HorizonParameterss.clear();
    }

    int size(){
        return m_horizons.size();
    }

    bool contains( QString name ){
        return m_horizons.contains(name);
    }

    HorizonParameters params(QString name){
        return m_HorizonParameterss.value(name);
    }

    Grid2D<float>* horizon(QString name){
        return m_horizons.value(name).get();
    }

signals:

    void horizonsChanged();
    void paramsChanged( QString );

public slots:

    void add( QString, HorizonParameters, std::shared_ptr<Grid2D<float>>);
    void remove( QString );
    void setParams( QString, HorizonParameters );

private:

    QMap<QString, HorizonParameters> m_HorizonParameterss;
    QMap<QString, std::shared_ptr<Grid2D<float> > >  m_horizons;
};

#endif // HORIZONMANAGER_H
