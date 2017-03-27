#include "colorcompositeviewer.h"
#include "ui_colorcompositeviewer.h"

#include<QMessageBox>

ColorCompositeViewer::ColorCompositeViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ColorCompositeViewer)
{
    ui->setupUi(this);
}

ColorCompositeViewer::~ColorCompositeViewer()
{
    delete ui;
}


void ColorCompositeViewer::setProject(AVOProject* project ){
    if( project==m_project) return;

    // reset grids because they are from other project
    m_red.reset();
    m_green.reset();
    m_blue.reset();
    refreshImage();
}

void ColorCompositeViewer::setRed(std::shared_ptr<Grid2D<float>> g){
    if( g==m_red) return;

    m_red=g;
    refreshImage();
}

void ColorCompositeViewer::setGreen(std::shared_ptr<Grid2D<float>> g){
    if( g==m_green ) return;

    m_green=g;
    refreshImage();
}

void ColorCompositeViewer::setBlue(std::shared_ptr<Grid2D<float>> g){
    if( g==m_blue ) return;

    m_blue=g;
    refreshImage();
}

void ColorCompositeViewer::refreshImage(){

    if( !m_red || !m_green || !m_blue ){
        //QMessageBox::critical(this, tr("Color Composite"), tr("Not all grids defined!"));
        return;
    }

    if( m_red->bounds()!=m_green->bounds() || m_red->bounds()!=m_blue->bounds()){
        //QMessageBox::critical(this, tr("Color Composite"), tr("Grids have different bounds!"));
        return;
    }

    auto redRange = valueRange(*m_red);
    auto greenRange = valueRange(*m_green);
    auto blueRange = valueRange(*m_blue);
    auto bounds=m_red->bounds();
    QImage img(bounds.height(), bounds.width(), QImage::Format_RGB32);
    img.fill(Qt::black);

    for( auto i = bounds.i1(); i<=bounds.i2(); i++){
        for( auto j=bounds.j1(); j<=bounds.j2(); j++){
            auto r = m_red->valueAt(i,j);
            if( r==m_red->NULL_VALUE) continue;
            auto rc = 255 * ( r - redRange.first )/(redRange.second-redRange.first);

            auto g = m_green->valueAt(i,j);
            if( g==m_green->NULL_VALUE) continue;
            auto gc = 255 * ( g - greenRange.first )/(greenRange.second-greenRange.first);

            auto b = m_blue->valueAt(i,j);
            if( b==m_blue->NULL_VALUE) continue;
            auto bc = 255 * ( b - blueRange.first )/(blueRange.second-blueRange.first);

            img.setPixel( i - bounds.i1(), j - bounds.j1(), qRgb( rc, gc, bc ) );
        }
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(img).scaled(3*img.size()));

}
