#ifndef COLORCOMPOSITEVIEWER_H
#define COLORCOMPOSITEVIEWER_H

#include <QMainWindow>

#include <avoproject.h>
#include <grid2d.h>
#include<memory>

namespace Ui {
class ColorCompositeViewer;
}

class ColorCompositeViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ColorCompositeViewer(QWidget *parent = 0);
    ~ColorCompositeViewer();

public slots:
    void setProject(AVOProject* );
    void setRed(std::shared_ptr<Grid2D<float>>);
    void setGreen(std::shared_ptr<Grid2D<float>>);
    void setBlue(std::shared_ptr<Grid2D<float>>);

private:
    void refreshImage();

    Ui::ColorCompositeViewer *ui;

    AVOProject* m_project;

    std::shared_ptr<Grid2D<float>> m_red;
    std::shared_ptr<Grid2D<float>> m_green;
    std::shared_ptr<Grid2D<float>> m_blue;
};

#endif // COLORCOMPOSITEVIEWER_H
