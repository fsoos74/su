#ifndef VOLUMEVIEW_H
#define VOLUMEVIEW_H

#include <ruleraxisview.h>
#include <volume.h>
#include <memory>
#include <colortable.h>
#include <grid2d.h>
#include <wellpath.h>
#include <wellmarkers.h>
#include <table.h>
#include <histogram.h>
#include "displayoptions.h"
#include "statistics.h"
#include<QList>
#include<QMap>
#include<QColor>

namespace sliceviewer {

class VolumePicker;
class ViewItem;
class ViewItemModel;
class HorizonItem;
class VolumeItem;


class VolumeView : public RulerAxisView
{
    Q_OBJECT

public:

    enum class SliceType{ Inline, Crossline, Z };

    struct SliceDef{
        SliceType type;
        int value;
    };

    VolumeView(QWidget* parent);

    VolumePicker* picker()const{
        return m_picker;
    }

    Grid3DBounds bounds(){
        return m_bounds;
    }

    Qt::Orientation inlineOrientation()const{
        return mInlineOrientation;
    }

    SliceDef slice()const{
        return m_slice;
    }

    ViewItemModel* horizonItemModel()const{
        return mHorizonItemModel;
    }

    ViewItemModel* volumeItemModel()const{
        return mVolumeItemModel;
    }

    ViewItemModel* wellItemModel()const{
        return mWellItemModel;
    }

    ViewItemModel* markerItemModel()const{
        return mMarkerItemModel;
    }

    ViewItemModel* tableItemModel()const{
        return mTableItemModel;
    }

    DisplayOptions displayOptions()const{
        return mDisplayOptions;
    }

    bool validSlice(const SliceDef& d);
    int adjustToVolume(SliceType t, int v);

public slots:

    void setSlice(VolumeView::SliceDef);
    void setDisplayOptions(const DisplayOptions&);
    void setInlineOrientation(Qt::Orientation);

    void back();
    void setFlattenHorizon( std::shared_ptr<Grid2D<float>>);
    void setTransforms(QTransform xy_to_ilxl, QTransform ilxl_to_xy);

signals:
    void inlineOrientationChanged(Qt::Orientation);
    void  sliceChanged(VolumeView::SliceDef);
    void volumesChanged();

protected:
    void drawForeground(QPainter *painter, const QRectF &rect)override; // need to draw picks
    void refreshScene()override;

private slots:
    void updateBounds();
    void updateAxes();
    void refreshSceneCaller();
    void onVolumeItemModelChanged();

private:

    void renderVolumes(QGraphicsScene*);
    void renderVolumeIline(QGraphicsScene*, const VolumeItem&, int);
    void renderVolumeXline(QGraphicsScene*, const VolumeItem&, int);
    void renderVolumeTime(QGraphicsScene*, const VolumeItem&, int);
    QImage sharpen(QImage);
    void renderHorizons(QGraphicsScene*);
    void renderHorizonIline(QGraphicsScene*, const HorizonItem&, int);
    void renderHorizonXline(QGraphicsScene*, const HorizonItem&, int);
    void renderWells(QGraphicsScene*);
    void renderMarkers(QGraphicsScene*);
    void renderTables(QGraphicsScene*);
    void renderLastViewed(QGraphicsScene*);
    QTransform swappedIlineXlineTransform()const;
    double dz(int i, int j)const;  // shift z-value based on flattening if defined, return nan if not defined
    QPainterPath intersectHorizonInline(const Grid2D<float>& grid, int iline);
    QPainterPath intersectHorizonCrossline(const Grid2D<float>& grid, int xline);
    QVector<QPointF> intersectTableInline(const Table& table, int iline);
    QVector<QPointF> intersectTableCrossline(const Table& table, int xline);
    QVector<QPointF> intersectTableTime(const Table& table, int time);
    QPainterPath projectWellPathInline(const WellPath& wp, int iline);
    QPainterPath projectWellPathCrossline(const WellPath& wp, int xline);
    QLineF intersectSlices(const SliceDef& s1, const SliceDef& s2);




    QTransform m_xy_to_ilxl, m_ilxl_to_xy;
    Qt::Orientation mInlineOrientation=Qt::Vertical;
    VolumePicker* m_picker;

    SliceDef m_slice;

    ViewItemModel* mHorizonItemModel;
    ViewItemModel* mVolumeItemModel;
    ViewItemModel* mWellItemModel;
    ViewItemModel* mMarkerItemModel;
    ViewItemModel* mTableItemModel;
    DisplayOptions mDisplayOptions;

    Grid3DBounds m_bounds;
    std::shared_ptr<Grid2D<float>> m_flattenHorizon;
    std::pair<double,double> m_flattenRange;

    // buffer statistics per volume to avoid unnecessary computations
    QMap<QString, statistics::Statistics >mVolumeStatistics;
    // keep track of previous slices for go back functionality
    QList<SliceDef> m_sliceList;
};

}

#endif // VOLUMEVIEW_H
