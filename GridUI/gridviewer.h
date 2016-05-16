#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<QScrollArea>
#include<QLabel>
#include<QSettings>
#include<QDir>

#include<memory>
#include<grid2d.h>
#include<gridview.h>
#include<colortable.h>
#include<colorbarwidget.h>
#include<displayrangedialog.h>
#include<isolinedialog.h>
#include "griddisplayoptionsdialog.h"
#include "orientationdialog.h"
#include "displayrangedialog.h"

#include <projectgeometry.h>

#include<baseviewer.h>
#include<selectionpoint.h>

namespace Ui {
class GridViewer;
}

class GridViewer : public BaseViewer
{
    Q_OBJECT

public:
    explicit GridViewer(QWidget *parent = 0);
    ~GridViewer();

    bool orientate(const ProjectGeometry&);

public slots:
    void setGrid( std::shared_ptr<Grid2D<double> >);

protected:

    void receivePoint( SelectionPoint );
    void receivePoints( QVector<SelectionPoint>, int code);

    void closeEvent(QCloseEvent*);

private slots:

    void onViewPointSelected( QPoint );     // need this to forward point from view to dispatcher

    void onViewPolylineSelected( QVector<QPoint>);  // see above

    void onGridViewMouseOver( int, int );


    void on_zoomInAct_triggered();

    void on_zoomOutAct_triggered();

    void on_zoomNormalAct_triggered();

    void on_displayRangeAct_triggered();

    void on_EditColorTableAct_triggered();

    void on_setBackgroundColorAct_triggered();

    void on_actionSetup_Contours_triggered();

    void on_actionAxxis_Orientation_triggered();

    void on_action_Grid_Display_triggered();

private:

    void setDefaultColorTable();

    void saveSettings();
    void loadSettings();

    Ui::GridViewer *ui;
    GridView* gridView;
    ColorBarWidget* colorBar;

    std::shared_ptr<Grid2D<double> > m_grid;

    OrientationDialog* orientationDialog=nullptr;
    DisplayRangeDialog* displayRangeDialog=nullptr;
    IsoLineDialog* isoLineDialog=nullptr;
    GridDisplayOptionsDialog* gridDisplayOptionsDialog=nullptr;
};

#endif // MAINWINDOW_H
