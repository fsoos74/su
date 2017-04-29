#ifndef COLORCOMPOSITEVIEWER_H
#define COLORCOMPOSITEVIEWER_H

#include<baseviewer.h>
#include <QDialog>
#include <QGraphicsPixmapItem>
#include <avoproject.h>
#include <grid2d.h>
#include<memory>


namespace Ui {
class ColorCompositeViewer;
}

class ColorCompositeViewer : public BaseViewer
{
    Q_OBJECT

public:
    explicit ColorCompositeViewer(QWidget *parent = 0);
    ~ColorCompositeViewer();

public slots:
    void setProject(AVOProject* );

protected:
    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);


private slots:
    void onMouseOver(QPointF);
    void setRed(std::shared_ptr<Grid2D<float>>);
    void setGreen(std::shared_ptr<Grid2D<float>>);
    void setBlue(std::shared_ptr<Grid2D<float>>);
    void updateInputGrids();
    void updateScene();

    void on_slRed_valueChanged(int value);
    void on_slGreen_valueChanged(int value);
    void on_slBlue_valueChanged(int value);

    void on_cbRed_currentIndexChanged(const QString &arg1);
    void on_cbGreen_currentIndexChanged(const QString &arg1);
    void on_cbBlue_currentIndexChanged(const QString &arg1);

private:
    std::pair<Grid2DBounds, QImage> buildImage( Grid2D<float>* red, Grid2D<float>* green, Grid2D<float>* blue);

    Ui::ColorCompositeViewer *ui;

    AVOProject* m_project;

    std::shared_ptr<Grid2D<float>> m_red;
    std::shared_ptr<Grid2D<float>> m_green;
    std::shared_ptr<Grid2D<float>> m_blue;

    QGraphicsPixmapItem* m_pixmapItem=nullptr;

};

#endif // COLORCOMPOSITEVIEWER_H
