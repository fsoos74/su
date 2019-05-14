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

#include<QList>
#include<QMap>
#include<QColor>

class VolumePicker;
//class VolumeItem;
//class VolumeItemModel;
class ViewItem;
class ViewItemModel;

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

    int welViewDist()const{
        return m_wellViewDist;
    }

    int sharpenPercent()const{
        return m_sharpenPercent;
    }

    int sharpenKernelSize()const{
        return m_sharpenKernelSize;
    }

    SliceDef slice()const{
        return m_slice;
    }

    QColor lastViewedColor()const{
        return m_lastViewedColor;
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
    QStringList tableList();
    QColor tableColor(QString);

    bool displayHorizons()const{
        return m_displayHorizons;
    }

    bool displayWells()const{
        return m_displayWells;
    }

    bool displayLasViewed()const{
        return m_displayLastViewed;
    }

    bool displayMarkers()const{
        return m_displayMarkers;
    }

    bool displayTables()const{
        return m_displayTables;
    }

    bool validSlice(const SliceDef& d);
    int adjustToVolume(SliceType t, int v);

    static QString toQString(SliceType);
    static SliceType toSliceType(QString);

public slots:

    void setSlice(SliceType,int);
    void setSlice(VolumeView::SliceDef);
    void setWellViewDist(int);
    void setSharpenPercent(int);
    void setSharpenKernelSize(int);

    void addTable(QString, std::shared_ptr<Table>, QColor);
    void removeTable(QString);
    void setTableColor(QString, QColor);

    void setLastViewedColor(QColor);
    void setDisplayHorizons(bool);
    void setDisplayWells(bool);
    void setDisplayMarkers(bool);
    void setDisplayTables(bool);
    void setDisplayLastViewed(bool);

    void back();
    void setFlattenHorizon( std::shared_ptr<Grid2D<float>>);
    void setTransforms(QTransform xy_to_ilxl, QTransform ilxl_to_xy);

signals:
    void horizonsChanged();
    void volumesChanged();
    void sliceChanged(VolumeView::SliceDef);
    void displayWellsChanged(bool);
    void displayMarkersChanged(bool);
    void displayHorizonsChanged(bool);
    void displayTablesChanged(bool);
    void displayLastViewedChanged(bool);

protected:
    void drawForeground(QPainter *painter, const QRectF &rect)override; // need to draw picks
    void refreshScene()override;

private slots:
    void updateBounds();
    void updateAxes();
    void refreshSceneCaller();
    void onVolumeItemModelChanged();

private:

    void renderSlice(QGraphicsScene*);
    void renderHorizons(QGraphicsScene*);
    void renderWells(QGraphicsScene*);
    void renderMarkers(QGraphicsScene*);
    void renderTables(QGraphicsScene*);
    void renderLastViewed(QGraphicsScene*);

    double dz(int i, int j)const;  // shift z-value based on flattening if defined, return nan if not defined

    struct TableItem{
        std::shared_ptr<Table> table;
        QColor color;
    };

    QImage intersectVolumeInline(const Volume& volume, ColorTable* colorTable, int iline, double ft, double lt);
    QImage intersectVolumeCrossline(const Volume& volume, ColorTable* colorTable, int xline, double ft, double lt);
    QImage intersectVolumeTime(const Volume& volume, ColorTable* colorTable, int time);
    QPainterPath intersectHorizonInline(const Grid2D<float>& grid, int iline);
    QPainterPath intersectHorizonCrossline(const Grid2D<float>& grid, int xline);
    QVector<QPointF> intersectTableInline(const Table& table, int iline);
    QVector<QPointF> intersectTableCrossline(const Table& table, int xline);
    QVector<QPointF> intersectTableTime(const Table& table, int time);
    QPainterPath projectWellPathInline(const WellPath& wp, int iline);
    QPainterPath projectWellPathCrossline(const WellPath& wp, int xline);
    QLineF intersectSlices(const SliceDef& s1, const SliceDef& s2);

    QImage enhance(QImage);
    void fillSceneInline(QGraphicsScene*);
    void fillSceneCrossline(QGraphicsScene*);
    void fillSceneTime(QGraphicsScene*);

    QTransform m_xy_to_ilxl, m_ilxl_to_xy;

    VolumePicker* m_picker;

    SliceDef m_slice;
    int m_wellViewDist=100;
    int m_sharpenPercent=0;
    int m_sharpenKernelSize=3;
    ViewItemModel* mHorizonItemModel;
    ViewItemModel* mVolumeItemModel;
    ViewItemModel* mWellItemModel;
    ViewItemModel* mMarkerItemModel;
    QMap<QString, TableItem> m_tables;         // picks/points
    QColor m_lastViewedColor=Qt::lightGray;
    bool m_displayHorizons=true;
    bool m_displayWells=true;
    bool m_displayMarkers=true;
    bool m_displayTables=true;
    bool m_displayLastViewed=true;
    Grid3DBounds m_bounds;
    std::shared_ptr<Grid2D<float>> m_flattenHorizon;
    std::pair<double,double> m_flattenRange;

    QList<SliceDef> m_sliceList;
};

#endif // VOLUMEVIEW_H
