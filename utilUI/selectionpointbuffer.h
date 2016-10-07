#ifndef SELECTIONPOINTBUFFER_H
#define SELECTIONPOINTBUFFER_H

#include <QObject>
#include <QVector>
#include <QMap>
#include "selectionpoint.h"

class BaseViewer;

class SelectionPointBuffer : public QObject
{
    Q_OBJECT
public:
    explicit SelectionPointBuffer(QObject *parent = 0);

    QVector<SelectionPoint> points( BaseViewer* v ){
        return m_points.value( v, QVector<SelectionPoint>() );
    }

    QList<BaseViewer*> viewers(){
        return m_points.keys();
    }

    QVector<SelectionPoint> intersectionPoints( QVector<SelectionPoint> );

signals:

    void pointsChanged(BaseViewer*);

public slots:

    void setPoints( BaseViewer*, QVector<SelectionPoint> );

private:

    QMap< BaseViewer*, QVector<SelectionPoint> > m_points;
};

#endif // SELECTIONPOINTBUFFER_H
