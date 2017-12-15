#ifndef BASEVIEWER_H
#define BASEVIEWER_H

#include <QMainWindow>

#include<memory>
#include "pointdispatcher.h"

class BaseViewer : public QMainWindow
{
friend class PointDispatcher;
    Q_OBJECT
public:
    explicit BaseViewer(QWidget *parent = 0);

    virtual ~BaseViewer();

    bool receptionEnabled()const{
        return m_receptionEnabled;
    }

    bool broadcastEnabled()const{
        return m_broadcastEnabled;
    }

    std::shared_ptr<PointDispatcher> dispatcher()const{
        return m_dispatcher;
    }

    QVector<qreal> intersectionTimes()const{
        return m_intersectionTimes;
    }

    QVector<SelectionPoint> intersectionPoints()const{
        return m_intersectionPoints;
    }

    void sendPoint(SelectionPoint, int code);
    void sendPoints(QVector<SelectionPoint>, int code);

signals:
    void intersectionTimesChanged( QVector<qreal>);
    void intersectionPointsChanged( QVector<SelectionPoint>);

protected:
    virtual void receivePoint( SelectionPoint, int code )=0;
    virtual void receivePoints( QVector<SelectionPoint>, int code)=0;

public slots:

    void setDispatcher(std::shared_ptr<PointDispatcher>);
    void setReceptionEnabled(bool);
    void setBroadcastEnabled(bool);
    void setIntersectionTimes(QVector<qreal>);
    void setIntersectionPoints(QVector<SelectionPoint>);

signals:

    void receptionEnabledChanged(bool);
    void broadcastEnabledChanged(bool);
    void shareCursorPositionChanged(bool);

private slots:


private:
    std::shared_ptr<PointDispatcher> m_dispatcher;
    bool m_receptionEnabled=true;
    bool m_broadcastEnabled=true;
    QVector<qreal> m_intersectionTimes;
    QVector<SelectionPoint> m_intersectionPoints;
};

#endif // BASEVIEWER_H
