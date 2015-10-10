#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<QScrollArea>
#include<QLabel>

#include<memory>
#include<grid2d.h>
#include<gridview.h>
#include<displayrangedialog.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openAct_triggered();

    void on_zoomInAct_triggered();

    void on_zoomOutAct_triggered();

    void on_zoomNormalAct_triggered();

    void on_displayRangeAct_triggered();

private:

    void setDefaultColorTable();

    Ui::MainWindow *ui;
    GridView* gridView;


    std::shared_ptr<Grid2D> m_horizon;


};

#endif // MAINWINDOW_H
