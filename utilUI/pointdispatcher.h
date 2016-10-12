#ifndef POINTDISPATCHER_H
#define POINTDISPATCHER_H


#include"selectionpoint.h"
#include<QVector>
#include<QSet>

class BaseViewer;


//enum PointCode{ VIEWER_CURRENT_CDP, VIEWER_CURRENT_ILINE, VIEWER_CURRENT_XLINE, CODE_SINGLE_POINTS=1, CODE_POLYGON, CODE_POLYLINE };
enum PointCode{ CODE_SINGLE_POINTS=1, CODE_POLYGON, CODE_POLYLINE, VIEWER_CURRENT_CDP, VIEWER_POINT_SELECTED, VIEWER_TIME_SELECTED };


class PointDispatcher
{
friend class BaseViewer;

public:

    PointDispatcher();

    int viewerCount()const{
        return m_viewers.size();
    }

    QVector<BaseViewer*> viewers()const;

    void sendPoint( SelectionPoint point, int code, BaseViewer* sender=nullptr);
    void sendPoints( QVector<SelectionPoint> points, int code, BaseViewer* sender=nullptr);

protected:

    void addViewer( BaseViewer* );
    void removeViewer( BaseViewer* );

private:

    QSet<BaseViewer*> m_viewers;
};

#endif // POINTDISPATCHER_H
