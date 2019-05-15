#ifndef VOLUMEPICKER_H
#define VOLUMEPICKER_H

#include <QObject>
#include <volume.h>
#include <table.h>
#include <memory>
#include <functional>
#include <volumeview.h>

namespace sliceviewer{

class VolumePicker : public QObject
{
    Q_OBJECT
public:

    enum class PickMode{ Points, Lines, Outline};

    explicit VolumePicker(VolumeView* view);

    VolumeView* view()const{
        return m_view;
    }

    std::shared_ptr<Table> picks()const{
        return m_picks;
    }

    bool isDirty()const{
        return m_dirty;
    }

    PickMode pickMode()const{
        return m_pickMode;
    }

signals:

    void pickModeChanged(VolumePicker::PickMode);
    void picksChanged();

public slots:

    void setPickMode(VolumePicker::PickMode);
    void setPickMode(QString);

    void newPicks();        // always multi, body
    void setPicks( std::shared_ptr<Table> );

    void pickPoint(QPointF);
    void pickLines(QPolygonF);
    void pickPolygon(QPolygonF);
    void removePoint(QPointF);

    void setDirty(bool);

private:

    void fillLinesIline(QPolygonF);
    void fillLinesXline(QPolygonF);
    void fillLinesZ(QPolygonF);
    void fillOutlineIline(QPolygonF);
    void fillOutlineXline(QPolygonF);
    void fillOutlineZ(QPolygonF);

    VolumeView* m_view;
    std::shared_ptr<Table>  m_picks;

    bool m_dirty;
    PickMode m_pickMode;
};


QString toQString(VolumePicker::PickMode);
VolumePicker::PickMode toVolumePickMode(QString);
QStringList volumePickModeNames();
QList<VolumePicker::PickMode> volumePickModes();

}

#endif // VOLUMEPICKER_H
