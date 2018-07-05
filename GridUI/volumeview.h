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

    QStringList volumeList();
    std::shared_ptr<Volume> volume(QString name);
    qreal volumeAlpha(QString name);
    ColorTable* volumeColorTable(QString name);

    QStringList horizonList();
    QColor horizonColor(QString);

    QStringList wellList();
    QColor wellColor()const{
        return m_wellColor;
    }

    QStringList markersList();
    QColor markerColor()const{
        return m_markerColor;
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

    QStringList displayedMarkers()const{
        return m_displayedMarkers;
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

    void addVolume(QString name, std::shared_ptr<Volume> );
    void removeVolume(QString name);
    void setVolumeAlpha(QString name, double);

    void addHorizon(QString name, std::shared_ptr<Grid2D<float>>, QColor);
    void removeHorizon(QString);
    void setHorizonColor(QString, QColor);

    void addWell( QString name, std::shared_ptr<WellPath>, std::shared_ptr<WellMarkers>);
    void removeWell(QString);
    void setWellColor(QColor);
    void setMarkerColor(QColor);

    void addTable(QString, std::shared_ptr<Table>, QColor);
    void removeTable(QString);
    void setTableColor(QString, QColor);

    void setDisplayedMarkers(QStringList);

    void setLastViewedColor(QColor);
    void setDisplayHorizons(bool);
    void setDisplayWells(bool);
    void setDisplayMarkers(bool);
    void setDisplayMarkerLabels(bool);
    void setDisplayTables(bool);
    void setDisplayLastViewed(bool);

    void back();
    void setFlattenHorizon( std::shared_ptr<Grid2D<float>>);
    void setTransforms(QTransform xy_to_ilxl, QTransform ilxl_to_xy);

signals:
    void volumesChanged();
    void sliceChanged(VolumeView::SliceDef);
    void displayWellsChanged(bool);
    void displayMarkersChanged(bool);
    void displayMarkerLabelsChanged(bool);
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

private:

    void renderSlice(QGraphicsScene*);
    void renderHorizons(QGraphicsScene*);
    void renderWells(QGraphicsScene*);
    void renderMarkers(QGraphicsScene*);
    void renderTables(QGraphicsScene*);
    void renderLastViewed(QGraphicsScene*);

    double dz(int i, int j)const;  // shift z-value based on flattening if defined, return nan if not defined

    struct VolumeItem{
        std::shared_ptr<Volume> volume;
        qreal alpha;
        ColorTable* colorTable;
    };

    struct HorizonItem{
        std::shared_ptr<Grid2D<float>> grid;
        QColor color;
    };

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
    QVector<std::pair<QPointF, QString>> projectMarkersInline(int iline);
    QVector<std::pair<QPointF, QString>> projectMarkersCrossline(int xline);
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
    QMap<QString, VolumeItem> m_volumes;
    QMap<QString, HorizonItem> m_horizons;
    QMap<QString, std::shared_ptr<WellPath>> m_wells;
    QMap<QString, std::shared_ptr<WellMarkers>> m_markers;
    QMap<QString, TableItem> m_tables;         // picks/points
    QStringList m_displayedMarkers;
    QColor m_wellColor=Qt::white;
    QColor m_markerColor=Qt::darkRed;
    QColor m_lastViewedColor=Qt::lightGray;
    bool m_displayHorizons=true;
    bool m_displayWells=true;
    bool m_displayMarkers=true;
    bool m_displayMarkerLabels=true;
    bool m_displayTables=true;
    bool m_displayLastViewed=true;
    Grid3DBounds m_bounds;
    std::shared_ptr<Grid2D<float>> m_flattenHorizon;
    std::pair<double,double> m_flattenRange;

    QList<SliceDef> m_sliceList;
};

#endif // VOLUMEVIEW_H
