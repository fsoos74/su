#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QPainterPath>
#include <QResizeEvent>
#include <wellpath.h>
#include <log.h>
#include <zmode.h>
#include <axis.h>
#include <memory>
#include <functional>
#include <QRubberBand>

#include<axisview.h>

class TrackView : public AxisView
{
Q_OBJECT

public:

    struct Marker{
        enum class Type{ Horizon, Top };
        Type type;
        QString name;
        qreal md;
    };

    TrackView(QWidget* parent);

    std::shared_ptr<Log> log(){
        return m_log;
    }

    std::shared_ptr<WellPath> wellPath(){
        return m_wellPath;
    }


    ZMode zmode()const{
        return m_zmode;
    }

    qreal zshift()const{
        return m_zshift;
    }

    int filterLen()const{
        return m_filterLen;
    }

    QStringList markers()const{
        return m_markers.keys();
    }

    QStringList markers(Marker::Type)const;

    qreal minZ()const;
    qreal maxZ()const;

    qreal log2view(qreal)const;
    qreal view2log(qreal)const;

public slots:

    void setLog( std::shared_ptr<Log> );
    void setWellPath( std::shared_ptr<WellPath>);
    void setHighlighted(QVector<qreal>);
    void setZMode( ZMode);
    void setZShift(qreal);  // this will be added to z-values, used to flatten on horizon
    void setFilterLen(int);
    void addMarker(Marker);
    void addMarker(Marker::Type,QString, qreal);
    void removeMarker(QString);
    void removeMarkers(Marker::Type);
    void removeMarkers();

protected:
    void refreshScene()override;

private slots:
    void setZFunc( std::function<double(double)>, std::function<double(double)>);
    void updateZFunc();

private:

    QPainterPath buildPath(Log log);

    std::shared_ptr<Log> m_log;
    std::shared_ptr<WellPath> m_wellPath;
    QVector<qreal> m_highlighted;
    QMap<QString, Marker> m_markers;
    ZMode m_zmode=ZMode::MD;
    std::function<double(double)> m_zfunc=[](double z){return z;};  // z values from log are passed to this function before being used, could use this to convert md to tvd
    std::function<double(double)> m_izfunc=[](double z){return z;}; // inverse function of above
    qreal m_zshift=0;
    int m_filterLen=0;
};

#endif // TRACKVIEW_H
