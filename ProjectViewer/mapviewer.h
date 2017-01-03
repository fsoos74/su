#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTransform>
#include <memory>
#include<projectgeometry.h>
#include<avoproject.h>

#include<colortable.h>
#include<colortabledialog.h>
#include<displayrangedialog.h>
#include<memory>

namespace Ui {
class MapViewer;
}

class MapViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapViewer(QWidget *parent = 0);
    ~MapViewer();

    void setProject(std::shared_ptr<AVOProject> );

    void setGrid( std::shared_ptr<Grid2D<float>>);

    bool isColorMappingLocked()const{
        return m_colorMappingLocked;
    }

    const QFont& tickFont()const{
        return m_tickFont;
    }

    const QPen& tickPen()const{
        return m_tickPen;
    }

    const QFont& lineFont()const{
        return m_lineFont;
    }

public slots:

    void setColorMappingLocked(bool);
    void setTickFont(const QFont&);
    void setTickPen(const QPen&);
    void setLineFont(const QFont&);

private slots:
    void refreshMap();

    void on_actionOpen_Grid_triggered();

    void on_action_Print_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionEdit_Colortable_triggered();

    void on_actionGrid_Display_Range_triggered();

    void on_actionClose_Grid_triggered();

    void on_action_Fit_Window_triggered();

    void on_actionCoordinate_Font_triggered();

    void on_actionLines_Font_triggered();

private:
    Ui::MapViewer *ui;

    void layoutPlot();
    void computeTransforms();

    void drawCoords( QGraphicsScene* );
    void drawGeometry( QGraphicsScene* );
    void drawGrid( QGraphicsScene* );
    void drawColorbar( QGraphicsScene*);

    QTransform m_ilxl_to_xy;
    QTransform m_ilxl_to_plot;
    QTransform m_xy_to_ilxl;
    QTransform m_xy_to_plot;

    QRectF m_bbPlot;        // bounding rect in plot units(mm)
    QRectF m_bbMap;         // bounding rect of map on plot in plot units(mm)
    QRectF m_bbColorbar;    // bounding box of colorbar on plot in plot units

    QRectF m_bbCoords;      // bounding rect of plot in coordinates
    QRect  m_bbProject;     // bounding rect of proect(from geometry) in inline/xline

    std::shared_ptr<AVOProject> m_project;

    std::shared_ptr<Grid2D<float>> m_grid;

    QFont m_tickFont;
    QPen  m_tickPen;
    QFont m_lineFont;

    ColorTable* m_colorTable;

    bool m_colorMappingLocked=false;

    DisplayRangeDialog* displayRangeDialog=nullptr;

    qreal m_scale=1;
};

#endif // MAPVIEWER_H
