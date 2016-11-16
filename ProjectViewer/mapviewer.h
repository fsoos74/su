#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QMainWindow>
#include <memory>
#include<projectgeometry.h>
#include<avoproject.h>

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

private slots:
    void refreshMap();

    void on_actionOpen_Grid_triggered();

    void on_action_Print_triggered();

private:
    Ui::MapViewer *ui;

    std::shared_ptr<AVOProject> m_project;

    std::shared_ptr<Grid2D<float>> m_grid;
};

#endif // MAPVIEWER_H
