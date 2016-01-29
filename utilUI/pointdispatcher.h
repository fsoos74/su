#ifndef POINTDISPATCHER_H
#define POINTDISPATCHER_H


#include<QPoint>
#include<QVector>
#include<QSet>

class BaseViewer;


enum PointCode{ CODE_SINGLE_POINTS=1, CODE_POLYGON, CODE_POLYLINE };

class PointDispatcher
{
friend class BaseViewer;

public:

    PointDispatcher();

    int viewerCount()const{
        return m_viewers.size();
    }

    void sendPoint( QPoint point, BaseViewer* sender=nullptr);
    void sendPoints( QVector<QPoint> points, int code, BaseViewer* sender=nullptr);

protected:

    void addViewer( BaseViewer* );
    void removeViewer( BaseViewer* );

private:

    QSet<BaseViewer*> m_viewers;
};

#endif // POINTDISPATCHER_H
