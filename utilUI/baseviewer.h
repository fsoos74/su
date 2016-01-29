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

    void sendPoint(QPoint);
    void sendPoints(QVector<QPoint>, int code);

protected:
    virtual void receivePoint( QPoint )=0;
    virtual void receivePoints( QVector<QPoint>, int code)=0;

public slots:

    void setDispatcher(std::shared_ptr<PointDispatcher>);
    void setReceptionEnabled(bool);
    void setBroadcastEnabled(bool);

signals:

    void receptionEnabledChanged(bool);
    void broadcastEnabledChanged(bool);

private:
    std::shared_ptr<PointDispatcher> m_dispatcher;
    bool m_receptionEnabled=true;
    bool m_broadcastEnabled=true;
};

#endif // BASEVIEWER_H
