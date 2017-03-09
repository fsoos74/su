#include "volumeviewer.h"
#include "ui_volumeviewer.h"
#include <colortable.h>
#include <colortabledialog.h>
#include <modaldisplayrangedialog.h>
#include <volumedataselectiondialog.h>
#include<QInputDialog>
#include<QMessageBox>
#include<QColorDialog>
#include<histogramdialog.h>
#include<QColorDialog>
#include<QInputDialog>
#include <QPainter>
#include <QImage>
#include<QToolBar>

#include <cmath>
#include <algorithm>
#include <random>
#include<horizondef.h>


VolumeDimensions toVolumeDimensions(Grid3DBounds bounds){

    return VolumeDimensions(
        bounds.inline1(), bounds.inline2(), bounds.crossline1(), bounds.crossline2(),
                static_cast<int>(1000*bounds.ft()), static_cast<int>(1000*bounds.lt() )
    );
}

VolumeViewer::VolumeViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::VolumeViewer),
    m_horizonModel(new HorizonModel(this)),
    m_sliceModel(new SliceModel(this))
{
    ui->setupUi(this);


    connect( ui->actionShow_Volume_Outline, SIGNAL(toggled(bool)), this, SLOT(refreshView()) );
    connect( ui->actionShow_Compass, SIGNAL(toggled(bool)), this, SLOT(refreshView()) );
    connect( ui->actionShow_Labels, SIGNAL(toggled(bool)), this, SLOT(refreshView()) );
    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    ui->openGLWidget->setTieXZScales( ui->actionTie_Scales->isChecked());
    connect( ui->actionTie_Scales, SIGNAL(toggled(bool)), ui->openGLWidget, SLOT(setTieXZScales(bool)) );

    connect( m_horizonModel, SIGNAL(changed()), this, SLOT(refreshView()) );
    connect( m_sliceModel, SIGNAL(changed()), this, SLOT(refreshView()) );


    // need this to scale highlighted points accordingly
    connect( ui->openGLWidget, SIGNAL(scaleChanged(QVector3D)), this, SLOT(refreshView()) );

    createDockWidgets();
    populateWindowMenu();
}

VolumeViewer::~VolumeViewer()
{
    delete ui;
}


void VolumeViewer::populateWindowMenu(){



    ui->menu_Window->addAction( m_navigationControlsDock->toggleViewAction());
    ui->menu_Window->addSeparator();
    ui->menu_Window->addAction( ui->toolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->toolBar_View->toggleViewAction());
}

void VolumeViewer::createDockWidgets(){

    m_navigationControlsDock = new QDockWidget(tr("Navigation Controls"), this);
    m_navigationControlsDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    m_navigationControlsDock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar |
                                      QDockWidget::DockWidgetClosable |
                                      QDockWidget::DockWidgetMovable |
                                      QDockWidget::DockWidgetFloatable);
    m_navigationControls=new Navigation3DControls(m_navigationControlsDock);
    m_navigationControlsDock->setWidget(m_navigationControls);
    addDockWidget(Qt::LeftDockWidgetArea, m_navigationControlsDock);
    Navigation3DControls* controls = m_navigationControls;

    connect( controls, SIGNAL(moveLeft()), ui->openGLWidget, SLOT(moveXNeg()) );
    connect( controls, SIGNAL(moveRight()), ui->openGLWidget, SLOT(moveXPos()) );
    connect( controls, SIGNAL(moveUp()), ui->openGLWidget, SLOT(moveYNeg()) );
    connect( controls, SIGNAL(moveDown()), ui->openGLWidget, SLOT(moveYPos()) );
    connect( controls, SIGNAL(moveBack()), ui->openGLWidget, SLOT(moveZPos()) );
    connect( controls, SIGNAL(moveFront()), ui->openGLWidget, SLOT(moveZNeg()) );

    connect( controls, SIGNAL(rotateXNeg()), ui->openGLWidget, SLOT(rotateXNeg()) );
    connect( controls, SIGNAL(rotateXPos()), ui->openGLWidget, SLOT(rotateXPos()) );
    connect( controls, SIGNAL(rotateYNeg()), ui->openGLWidget, SLOT(rotateYNeg()) );
    connect( controls, SIGNAL(rotateYPos()), ui->openGLWidget, SLOT(rotateYPos()) );
    connect( controls, SIGNAL(rotateZNeg()), ui->openGLWidget, SLOT(rotateZNeg()) );
    connect( controls, SIGNAL(rotateZPos()), ui->openGLWidget, SLOT(rotateZPos()) );

    connect( controls, SIGNAL(scaleXNeg()), ui->openGLWidget, SLOT(scaleXNeg()) );
    connect( controls, SIGNAL(scaleXPos()), ui->openGLWidget, SLOT(scaleXPos()) );
    connect( controls, SIGNAL(scaleYNeg()), ui->openGLWidget, SLOT(scaleYNeg()) );
    connect( controls, SIGNAL(scaleYPos()), ui->openGLWidget, SLOT(scaleYPos()) );
    connect( controls, SIGNAL(scaleZNeg()), ui->openGLWidget, SLOT(scaleZNeg()) );
    connect( controls, SIGNAL(scaleZPos()), ui->openGLWidget, SLOT(scaleZPos()) );

}

void VolumeViewer::setProject(AVOProject* p){

    if( p==m_project ) return;

    m_project=p;

    if( m_project ){
        m_project->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);
    }
    else{
        xy_to_ilxl=ilxl_to_xy=QTransform();
    }
    //update();
}


void VolumeViewer::setDimensions(VolumeDimensions dim){

    if( dim==m_dimensions ) return;

    m_dimensions=dim;

    refreshView();

    emit dimensionsChanged(dim);
}


void VolumeViewer::addVolume(QString name, std::shared_ptr<Grid3D<float> > volume){

    if( m_volumes.contains(name)) return;   // no dupes

    if( name.isEmpty() || !volume ) return; // check key and volume are valid

    // create colortable first

    ColorTable* ct=new ColorTable(this);
    // NEED ADD CHECK FOR ALLOC OK
    ct->setColors(ColorTable::defaultColors());
    ct->setRange(volume->valueRange());
    connect( ct, SIGNAL(colorsChanged()), this, SLOT(refreshView()) );
    connect( ct, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refreshView()) );
    connect( ct, SIGNAL(powerChanged(double)), this, SLOT(refreshView()) );

    QDockWidget* volumeColorBarDock = new QDockWidget(name, this);
    volumeColorBarDock->setAllowedAreas(Qt::RightDockWidgetArea);
    volumeColorBarDock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar |
                                      QDockWidget::DockWidgetClosable |
                                      QDockWidget::DockWidgetMovable |
                                      QDockWidget::DockWidgetFloatable);
    ColorBarWidget* volumeColorBarWidget=new ColorBarWidget(volumeColorBarDock);
    volumeColorBarWidget->setFixedHeight(200);
    volumeColorBarWidget->setScaleAlignment(ColorBarWidget::SCALE_LEFT);
    volumeColorBarDock->setContentsMargins(10, 5, 10, 5);
    volumeColorBarDock->setWidget(volumeColorBarWidget);
    volumeColorBarWidget->setColorTable(ct);

    m_colorTables.insert(name, ct);
    m_volumeColorBarDocks.insert(name, volumeColorBarDock);
    m_volumeColorBarWidgets.insert(name, volumeColorBarWidget);
    m_volumes.insert(name, volume);

    addDockWidget(Qt::RightDockWidgetArea, volumeColorBarDock);
    volumeColorBarDock->close();

    // add colorbar to window menu
    ui->menu_Window->addAction( volumeColorBarDock->toggleViewAction());

    // FIRST VOLUME CREATE DEFAULT SLICES
    if( m_volumes.size()==1) initialVolumeDisplay();

    emit volumeListChanged(volumeList());
}


void VolumeViewer::removeVolume(QString name){

    if( name.isEmpty() ) return;

    // remove colorbar from window menu
    QDockWidget* dock=m_volumeColorBarDocks.value( name, nullptr );
    if( dock ){
        ui->menu_Window->removeAction(dock->toggleViewAction());
        removeDockWidget(dock);
        delete dock;      // when dock is deleted also cb widget will be deleted because it is child of dock
    }
    m_volumeColorBarDocks.remove(name);

    HistogramRangeSelectionDialog* dlg=m_displayRangeDialogs.value(name, nullptr);
    if(dlg){
        dlg->close();
        delete dlg;
    }
    m_displayRangeDialogs.remove(name);

    ColorTable* ct=m_colorTables.value(name, nullptr);
    if( ct ){
        delete ct;
    }
    m_colorTables.remove(name);

    m_volumes.remove(name);

    refreshView();

    emit volumeListChanged(volumeList());
}


void VolumeViewer::setHighlightedPoints(QVector<SelectionPoint> rpoints){

    m_highlightedPoints=rpoints;

    refreshView();
}


void VolumeViewer::setHighlightedPointColor(QColor color){

    if( color==m_highlightedPointColor) return;

    m_highlightedPointColor=color;

    refreshView();
}

void VolumeViewer::setHighlightedPointSize(qreal s){

    if( s==m_highlightedPointSize) return;

    m_highlightedPointSize=s;

    refreshView();
}

void VolumeViewer::setCompassColor(QColor c){

    if( c==m_compassColor ) return;

    m_compassColor=c;

    refreshView();
}

void VolumeViewer::setCompassSize(qreal s){

    if( s==m_compassSize ) return;

    m_compassSize=s;

    refreshView();
}

void VolumeViewer::setLabelColor(QColor c){

    if( c==m_labelColor ) return;

    m_labelColor=c;

    refreshView();
}

void VolumeViewer::setLabelSize(qreal s){

    if( s==m_labelSize ) return;

    m_labelSize=s;

    refreshView();
}

void VolumeViewer::setOutlineColor(QColor c){

    if( c==m_outlineColor ) return;

    m_outlineColor=c;

    refreshView();
}

void VolumeViewer::clear(){

    m_sliceModel->clear();
    m_horizonModel->clear();
    m_highlightedPoints.clear();

    refreshView();
}


void VolumeViewer::refreshView(){

    ViewWidget* view=ui->openGLWidget;
    Q_ASSERT(view);


    ui->openGLWidget->makeCurrent(); // need this to access currect VBOs

    RenderScene* scene=view->scene();
    //if( !scene ) return;
    Q_ASSERT(scene);

    scene->clear();

    QVector<int> ilines;
    QVector<int> xlines;
    QVector<int> msecs;
    // volume outline
    if( ui->actionShow_Volume_Outline->isChecked()){
        outlineToView( m_dimensions, m_outlineColor);

        // add min/max inline and crossline and time to labels if outline is drawn
        ilines<<m_dimensions.inline1;
        ilines<<m_dimensions.inline2;
        xlines<<m_dimensions.crossline1;
        xlines<<m_dimensions.crossline2;
        msecs<<m_dimensions.msec1;
        msecs<<m_dimensions.msec2;
    }

    // slices
    foreach( QString name, m_sliceModel->names()){
        SliceDef slice=m_sliceModel->slice(name);
        sliceToView(slice);
        // add slice to label list
        switch(slice.type){
            case SliceType::INLINE: ilines<<slice.value;break;
            case SliceType::CROSSLINE: xlines<<slice.value;break;
            case SliceType::TIME: msecs<<slice.value;break;
        }
    }

    double ft=0.001*m_dimensions.msec1;
    double lt=0.001*m_dimensions.msec2;

    // labels
    if( ui->actionShow_Labels->isChecked()){

        // inlines
        foreach( int iline, ilines){

            QPointF xy1=ilxl_to_xy.map( QPoint(iline, m_dimensions.crossline1 ) );
            QPointF xy2=ilxl_to_xy.map( QPoint(iline, m_dimensions.crossline2 ) );
            QString str=QString("iline %1").arg(iline);
            textToView( QVector3D( xy1.x(), ft, xy1.y()), QVector3D( xy2.x(), ft, xy2.y()),
                        str, Qt::AlignHCenter|Qt::AlignBottom);
            textToView( QVector3D( xy2.x(), lt, xy2.y()), QVector3D( xy1.x(), lt, xy1.y()),
                        str, Qt::AlignHCenter|Qt::AlignTop);
        }


        // crosslines
        foreach( int xline, xlines){

            QPointF xy1=ilxl_to_xy.map( QPoint(m_dimensions.inline1, xline ) );
            QPointF xy2=ilxl_to_xy.map( QPoint(m_dimensions.inline2, xline ) );
            QString str=QString("xline %1").arg(xline);
            textToView( QVector3D( xy1.x(), ft, xy1.y()), QVector3D( xy2.x(), ft, xy2.y()),
                        str, Qt::AlignHCenter|Qt::AlignBottom);
            textToView( QVector3D( xy2.x(), lt, xy2.y()), QVector3D( xy1.x(), lt, xy1.y()),
                        str, Qt::AlignHCenter|Qt::AlignTop);
        }

        // times / msecs
        QPointF xy1=ilxl_to_xy.map( QPoint(m_dimensions.inline1, m_dimensions.crossline1 ) );
        QPointF xy2=ilxl_to_xy.map( QPoint(m_dimensions.inline1, m_dimensions.crossline2 ) );
        QPointF xy3=ilxl_to_xy.map( QPoint(m_dimensions.inline2, m_dimensions.crossline2 ) );
        QPointF xy4=ilxl_to_xy.map( QPoint(m_dimensions.inline2, m_dimensions.crossline1 ) );
        foreach( int msec, msecs){
            qreal t=0.001*msec;
            QString str=QString("%1 ms").arg(msec);
            textToView( QVector3D( xy1.x(), t, xy1.y()), QVector3D( xy3.x(), t, xy3.y()), str, Qt::AlignLeft | Qt::AlignVCenter);
            textToView( QVector3D( xy2.x(), t, xy2.y()), QVector3D( xy4.x(), t, xy4.y()), str, Qt::AlignLeft | Qt::AlignVCenter);
            textToView( QVector3D( xy3.x(), t, xy3.y()), QVector3D( xy1.x(), t, xy1.y()), str, Qt::AlignLeft | Qt::AlignVCenter);
            textToView( QVector3D( xy4.x(), t, xy4.y()), QVector3D( xy2.x(), t, xy2.y()), str, Qt::AlignLeft | Qt::AlignVCenter);
        }

    }


    // horizons
    foreach( QString hname, m_horizonModel->names() ){

        HorizonDef hdef=m_horizonModel->item(hname);
        horizonToView(hdef);
    }

    // highlighted points
    pointsToView( m_highlightedPoints, m_highlightedPointColor, m_highlightedPointSize );


    // compasses / north indicators
    if( ui->actionShow_Compass->isChecked()){
        // draw compasses on every corner at top and bottom of outline cube
        //Grid3Dm_dimensions m_dimensions=m_volume->m_dimensions();
        QVector<QPoint> compassPoints{ QPoint{ m_dimensions.inline1, m_dimensions.crossline1 },
                                       QPoint{ m_dimensions.inline1, m_dimensions.crossline2 },
                                       QPoint{ m_dimensions.inline2, m_dimensions.crossline1 },
                                       QPoint{ m_dimensions.inline2, m_dimensions.crossline2 } };
        foreach( QPoint ilxl, compassPoints){
            QPointF xy=ilxl_to_xy.map(ilxl);
            compassToView( QVector3D{static_cast<float>(xy.x()), static_cast<float>(ft), static_cast<float>(xy.y())},
                           m_compassSize, m_compassColor );
            compassToView( QVector3D{static_cast<float>(xy.x()), static_cast<float>(lt), static_cast<float>(xy.y())},
                           m_compassSize, m_compassColor );
        }
    }

    ui->openGLWidget->update();
}

void VolumeViewer::receivePoint( SelectionPoint point, int code ){
/*
    Q_ASSERT(m_volume);

    switch( code ){

    case PointCode::VIEWER_POINT_SELECTED:{

       if( point.iline!=SelectionPoint::NO_LINE){
            m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{m_volumeName, SliceType::INLINE, point.iline} );
        }
        if( point.xline!=SelectionPoint::NO_LINE){
            m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{m_volumeName, SliceType::CROSSLINE, point.xline} );
        }
        break;
    }

    case PointCode::VIEWER_TIME_SELECTED:{
        int msec=static_cast<int>(1000*point.time);
        m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{m_volumeName, SliceType::TIME, msec} );
        break;
    }
    default:{               // nop

        break;
    }

    }
*/
}

void VolumeViewer::receivePoints( QVector<SelectionPoint> points, int code){

    if( code==CODE_SINGLE_POINTS){

        if( points.size()<m_maxHighlightedPoints){
            setHighlightedPoints(points);
        }
        else{
            // randomly selected max points
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(points.begin(), points.end(), g);

            QVector<SelectionPoint> tmp;
            tmp.reserve(m_maxHighlightedPoints);
            std::copy( points.begin(), points.begin()+m_maxHighlightedPoints, std::back_inserter(tmp) );
            setHighlightedPoints(tmp);

            statusBar()->showMessage( QString("Highlighted Points limited to %1").arg(m_maxHighlightedPoints), 3000);
        }
    }

}

void VolumeViewer::outlineToView(VolumeDimensions dims, QColor color){

    QVector3D vcolor{ static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF() ) };



    QPoint ILXL1(dims.inline1, dims.crossline1);
    QPoint ILXL2(dims.inline1, dims.crossline2);
    QPoint ILXL3(dims.inline2, dims.crossline1);
    QPoint ILXL4(dims.inline2, dims.crossline2);

    QPointF XY1=ilxl_to_xy.map(ILXL1);
    QPointF XY2=ilxl_to_xy.map(ILXL2);
    QPointF XY3=ilxl_to_xy.map(ILXL3);
    QPointF XY4=ilxl_to_xy.map(ILXL4);

    float ft=0.001*dims.msec1;
    float lt=0.001*dims.msec2;
    QVector<VIC::Vertex> vertices{
        {QVector3D( XY1.x(), ft, XY1.y()), vcolor },
        {QVector3D( XY2.x(), ft, XY2.y()), vcolor },
        {QVector3D( XY4.x(), ft, XY4.y()), vcolor },
        {QVector3D( XY3.x(), ft, XY3.y()), vcolor },
        {QVector3D( XY1.x(), lt, XY1.y()), vcolor },
        {QVector3D( XY2.x(), lt, XY2.y()), vcolor },
        {QVector3D( XY4.x(), lt, XY4.y()), vcolor },
        {QVector3D( XY3.x(), lt, XY3.y()), vcolor }
    };

    // points for gl_lines, maybe change to line_strip
    QVector<VIC::Index> indices{
         0,1, 1,2, 2,3, 3,0,           // top
         4,5, 5,6, 6,7, 7,4,           // bottom
         0,1, 1,5, 5,4, 4,0,           // left
         1,5, 5,6, 6,2, 2,1,           // back
         2,6, 6,7, 7,3, 3,2,           // right
         3,7, 7,4, 4,0, 0,3            // front
    };

    ui->openGLWidget->scene()->addItem(VIC::makeVIC(vertices, indices, GL_LINES) );
}


void VolumeViewer::directionIndicatorPlanesToView( Grid3DBounds bounds){

    // add planes of min x=blue, min y=red, min z=green
    QRect br_ilxl( bounds.inline1(), bounds.crossline1(), bounds.inlineCount(), bounds.crosslineCount() );
    QRectF br_xy=ilxl_to_xy.mapRect(br_ilxl);

    qreal xmin=br_xy.left();
    qreal xmax=br_xy.right();
    qreal zmin=br_xy.top();
    qreal zmax=br_xy.bottom();
    qreal ymin=bounds.ft();
    qreal ymax=bounds.lt();

    // top (ymin) rectangle
    {
        QVector3D color{ 1., 0., 0. };
        QVector<VIC::Vertex> vertices={
            {QVector3D( xmin, ymin, zmin ), color },
            {QVector3D( xmax, ymin, zmin ), color },
            {QVector3D( xmax, ymin, zmax ), color },
            {QVector3D( xmin, ymin, zmax ), color }
        };

        QVector<VIC::Index> indices{ 0, 1, 2, 3};

        ui->openGLWidget->scene()->addItem(VIC::makeVIC(vertices, indices, GL_QUADS) );
    }

    // front (zmin) rectangle
    {
        QVector3D color{ 0., 1., 0. };
        QVector<VIC::Vertex> vertices={
            {QVector3D( xmin, ymin, zmin ), color },
            {QVector3D( xmax, ymin, zmin ), color },
            {QVector3D( xmax, ymax, zmin ), color },
            {QVector3D( xmin, ymax, zmin ), color }
        };

        QVector<VIC::Index> indices{ 0, 1, 2, 3};

        ui->openGLWidget->scene()->addItem(VIC::makeVIC(vertices, indices, GL_QUADS) );
    }

    // left (xmin) rectangle
    {
        QVector3D color{ 0., 0., 1. };
        QVector<VIC::Vertex> vertices={
            {QVector3D( xmin, ymin, zmin ), color },
            {QVector3D( xmin, ymin, zmax ), color },
            {QVector3D( xmin, ymax, zmax ), color },
            {QVector3D( xmin, ymax, zmin ), color }
        };

        QVector<VIC::Index> indices{ 0, 1, 2, 3};

        ui->openGLWidget->scene()->addItem(VIC::makeVIC(vertices, indices, GL_QUADS) );
    }
}

void VolumeViewer::sliceToView( const SliceDef& def ){

    switch(def.type){
    case SliceType::INLINE: inlineSliceToView(def);break;
    case SliceType::CROSSLINE: crosslineSliceToView(def); break;
    case SliceType::TIME: timeSliceToView(def); break;
    default: qFatal("Invalid SliceType!!!");
    }
}

QImage mix( QImage img1, QImage img2, int perc2){

    Q_ASSERT( img1.size()==img2.size());

    QImage tmp(img1.width(), img1.height(),  QImage::Format_RGBA8888 );
    tmp.fill(qRgba(0,0,0,0));
    QPainter painter(&tmp);
    painter.setOpacity( qreal(100-perc2)/100 );
    painter.drawImage(0,0,img1);
    painter.setOpacity( qreal(perc2)/100 );
    painter.drawImage(0,0,img2);
    return tmp;
}

// blending by percentage
QColor mix(QColor color1, QColor color2, qreal r){

    return QColor::fromRgbF( (1.-r)*color1.redF() + r * color2.redF(),
                    (1.-r)*color1.greenF() + r * color2.greenF(),
                    (1.-r)*color1.blueF() + r* color2.blueF(),
                    (1.-r)*color1.alphaF() + r* color2.alphaF()  );
}



void VolumeViewer::inlineSliceToView( const SliceDef& def){

    std::shared_ptr<Grid3D<float>> pvolume=m_volumes.value(def.volume);
    if( !pvolume) return;
    Grid3D<float>& volume=*pvolume;
    Grid3DBounds bounds=volume.bounds();
    VolumeDimensions dims=toVolumeDimensions(bounds) & m_dimensions;

    int iline=def.value;
    if( iline<dims.inline1 || iline>dims.inline2 ) return;

    ColorTable* colorTable=m_colorTables.value(def.volume, nullptr);
    if(!colorTable) return;

    int sample1=bounds.timeToSample(0.001*dims.msec1);
    int sample2=bounds.timeToSample(0.001*dims.msec2);
    int sampleCount=sample2-sample1+1;
    QImage img(dims.crosslineCount(), sampleCount, QImage::Format_RGBA8888);// QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){

            Grid3D<float>::value_type value=volume(iline, dims.crossline1+i, sample1+j);
            ColorTable::color_type color=(value!=volume.NULL_VALUE) ? colorTable->map(value) : qRgba(0,0,0,0);
            img.setPixel(i,j, color );
        }
    }

    QPointF xy1=ilxl_to_xy.map(QPoint(iline, dims.crossline1));
    QPointF xy2=ilxl_to_xy.map(QPoint(iline, dims.crossline2));
    float ft=0.001*dims.msec1;
    float lt=0.001*dims.msec2;
    QVector<VIT::Vertex> vertices{
        {QVector3D( xy1.x(), ft, xy1.y()), QVector2D( 0, 0)},   // top left
        {QVector3D( xy2.x(), ft, xy2.y()), QVector2D( 1, 0)},   // top right
        {QVector3D( xy1.x(), lt, xy1.y()), QVector2D( 0, 1)},   // bottom left
        {QVector3D( xy2.x(), lt, xy2.y()), QVector2D( 1, 1)}    // bottom right
    };

    QVector<VIT::Index> indices{
         0,  1,  2,  3
    };

    ui->openGLWidget->scene()->addItem(VIT::makeVIT(vertices, indices, GL_TRIANGLE_STRIP, img) );
}

void VolumeViewer::crosslineSliceToView(  const SliceDef& def){

    std::shared_ptr<Grid3D<float>> pvolume=m_volumes.value(def.volume);
    if( !pvolume) return;
    Grid3D<float>& volume=*pvolume;
    Grid3DBounds bounds=volume.bounds();
    VolumeDimensions dims=toVolumeDimensions(bounds) & m_dimensions;

    int xline=def.value;
    if( xline<dims.crossline1 || xline>dims.crossline2) return;

    ColorTable* colorTable=m_colorTables.value(def.volume, nullptr);
    if(!colorTable) return;

    int sample1=bounds.timeToSample(0.001*dims.msec1);
    int sample2=bounds.timeToSample(0.001*dims.msec2);
    int sampleCount=sample2-sample1+1;
    QImage img(dims.inlineCount(), sampleCount, QImage::Format_RGBA8888);// QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){

            Grid3D<float>::value_type value=volume(dims.inline1+i, xline, sample1+j);
            ColorTable::color_type color=(value!=volume.NULL_VALUE) ? colorTable->map(value) : qRgba(0,0,0,0);
            img.setPixel(i,j, color );

            //img.setPixel(i,j,m_colorTable->map(volume(bounds.inline1()+i, xline, j) ) );
        }
    }

    QPointF xy1=ilxl_to_xy.map(QPoint(dims.inline1, xline));
    QPointF xy2=ilxl_to_xy.map(QPoint(dims.inline2, xline));
    float ft=0.001*dims.msec1;
    float lt=0.001*dims.msec2;
    QVector<VIT::Vertex> vertices{
        {QVector3D( xy1.x(), ft, xy1.y()), QVector2D( 0, 0)},   // top left
        {QVector3D( xy2.x(), ft, xy2.y()), QVector2D( 1, 0)},   // top right
        {QVector3D( xy1.x(), lt, xy1.y()), QVector2D( 0, 1)},   // bottom left
        {QVector3D( xy2.x(), lt, xy2.y()), QVector2D( 1, 1)}    // bottom right
    };

    QVector<VIT::Index> indices{
         0,  1,  2,  3
    };

    ui->openGLWidget->scene()->addItem(VIT::makeVIT(vertices, indices, GL_TRIANGLE_STRIP, img) );

}

void VolumeViewer::timeSliceToView( const SliceDef& def ){

    std::shared_ptr<Grid3D<float>> pvolume=m_volumes.value(def.volume);
    if( !pvolume) return;
    Grid3D<float>& volume=*pvolume;
    Grid3DBounds bounds=volume.bounds();
    VolumeDimensions dims=toVolumeDimensions(bounds) & m_dimensions;

    int msec=def.value;
    qreal time=0.001*msec;
    if( msec<dims.msec1 || msec>dims.msec2  ) return;

    ColorTable* colorTable=m_colorTables.value(def.volume, nullptr);
    if(!colorTable) return;

    QImage img(dims.inlineCount(), dims.crosslineCount(), QImage::Format_RGBA8888); //QImage::Format_RGB32 );
    for( int il=dims.inline1; il<=dims.inline2; il++){
        for( int xl=dims.crossline1; xl<=dims.crossline2; xl++){
            int xi=il-dims.inline1;
            int yi=xl-dims.crossline1;

            Grid3D<float>::value_type value=volume.value(il, xl, time);
            ColorTable::color_type color=(value!=volume.NULL_VALUE) ? colorTable->map(value) : qRgba(0,0,0,0);
            img.setPixel(xi, yi, color );
        }
    }



    QPoint ILXL1(dims.inline1, dims.crossline1);
    QPoint ILXL2(dims.inline1, dims.crossline2);
    QPoint ILXL3(dims.inline2, dims.crossline1);
    QPoint ILXL4(dims.inline2, dims.crossline2);

    QPointF XY1=ilxl_to_xy.map(ILXL1);
    QPointF XY2=ilxl_to_xy.map(ILXL2);
    QPointF XY3=ilxl_to_xy.map(ILXL3);
    QPointF XY4=ilxl_to_xy.map(ILXL4);


    QVector<VIT::Vertex> vertices{
        {QVector3D( XY1.x(), time, XY1.y() ), QVector2D( 0, 0)},   // il1 xl1
        {QVector3D( XY2.x(), time, XY2.y() ), QVector2D( 0, 1)},   // il1 xl2
        {QVector3D( XY3.x(), time, XY3.y() ), QVector2D( 1, 0)},   // il2 xl1
        {QVector3D( XY4.x(), time, XY4.y() ), QVector2D( 1, 1)}    // il2 xl2
    };

    QVector<VIT::Index> indices{
         0,  1,  3,  2
    };

    ui->openGLWidget->scene()->addItem( VIT::makeVIT(vertices, indices, GL_QUADS, img ) );
}



// project volume on horizon
void VolumeViewer::horizonToView(const HorizonDef& hdef){

    if(!hdef.horizon) return;
    auto hrz = hdef.horizon.get();

    if(!m_volumes.contains(hdef.volume)) return;
    auto volume=m_volumes.value(hdef.volume).get();

    auto colorTable=m_colorTables.value(hdef.volume, nullptr);
    if( !colorTable ) return;

    Grid2DBounds hbounds=hrz->bounds();

    //auto range=valueRange(*hrz);
    QVector3D nullColor{0,0,0};
    QVector<VIC::Vertex> vertices;

    for( int iline=hbounds.i1(); iline<=hbounds.i2(); iline++){

        for( int xline=hbounds.j1(); xline<=hbounds.j2(); xline++){

            float tms=(*hrz)(iline, xline);  // horizon is in millis
            QPointF xy=ilxl_to_xy.map(QPoint(iline, xline));

            if(tms!=hrz->NULL_VALUE){
                // scale colour according to relative depth, deeper->darker
                //float x=(tms-range.first)/(range.second-range.first);
                qreal t = 0.001*(hdef.delay+tms);        // convert time to seconds

                QVector3D color = nullColor;

                float value = volume->value(iline, xline, t);

                if( value != volume->NULL_VALUE ){

                    QColor c = colorTable->map(value);
                    float red=static_cast<float>(c.redF());
                    float green=static_cast<float>(c.greenF());
                    float blue=static_cast<float>(c.blueF());
                    color = QVector3D{ red, green , blue };
                }

                vertices.append( VIC::Vertex{ QVector3D( xy.x(), t, xy.y() ), color } );
            }
            else{
                vertices.append( VIC::Vertex{ QVector3D( xy.x(), 0, xy.y() ), nullColor } );
            }

        }
    }

    QVector<VIC::Index> indices;

    bool gap=false;
    for( int i=hbounds.i1(); i<hbounds.i2(); i++){  // 1 less

        for( int j=hbounds.j1(); j<=hbounds.j2(); j++){

            int l=(i-hbounds.i1())*hbounds.width()+j-hbounds.j1();
            if( (*hrz)(i,j)!=hrz->NULL_VALUE){ //ignore null values
                if( gap ){
                    indices.append(l);
                    gap=false;
                }
                indices.append(l);
            }
            else{
                if(!gap && !indices.empty()){
                    indices.append(indices.back());
                    gap=true;
                }
            }

            int r=l+hbounds.width();
            if( (*hrz)(i+1,j)!=hrz->NULL_VALUE){ //ignore null values
                if( gap ){
                    indices.append(r);
                    gap=false;
                }
                indices.append(r);
            }
            else{
                if(!gap && !indices.empty()){
                    indices.append(indices.back());
                    gap=true;
                }
            }
        }
        if(!gap){
            indices.append(indices.back()); // duplicate point marks end of part
            gap=true;
        }
    }

    ui->openGLWidget->scene()->addItem( VIC::makeVIC(vertices, indices, GL_TRIANGLE_STRIP) );
}




void VolumeViewer::pointsToView(QVector<SelectionPoint> points, QColor color, qreal SIZE){


    QVector3D drawColor{static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF()) };
    QVector<VIC::Vertex> vertices;
    QVector<VIC::Index> indices;


    // reserve to accelerate

    QVector3D scale=ui->openGLWidget->scale();
    qreal sizeX=SIZE/std::fabs(scale.x());
    qreal sizeY=SIZE/std::fabs(scale.y());
    qreal sizeZ=SIZE/std::fabs(scale.z());

    for( int i=0; i<points.size(); i++){

        const SelectionPoint& point=points[i];

        QPointF xy=ilxl_to_xy.map(QPoint(point.iline, point.xline));
        QVector3D p( xy.x(), point.time, xy.y() );

        qreal fac=sqrt(3.)/3;//sqrt(2.)/2;

        qreal x=p.x();
        qreal xleft=x-fac*sizeX;
        qreal xright=x+fac*sizeX;
        qreal y=p.y();
        qreal ytop=y-sizeY;
        qreal ybottom=y+fac*sizeY;
        qreal z=p.z();
        qreal zfront=z-fac*sizeZ;
        qreal zback=z+fac*sizeZ;


        vertices.append( VIC::Vertex{ QVector3D( xleft, ybottom, zfront), drawColor} );  // 1
        vertices.append( VIC::Vertex{ QVector3D( x, ybottom, zback), drawColor} );       // 2
        vertices.append( VIC::Vertex{ QVector3D( xright, ybottom, zfront), drawColor} ); // 3
        vertices.append( VIC::Vertex{ QVector3D( x, ytop, z), drawColor} );              // 4


        int j=i*4;              // index of first vertex or this tetraeder
        indices.append(j);      // 1

        // we are not first:
        if( i>0 ){
            indices.append(indices.back());
        }

        indices.append(j+1);    // 2
        indices.append(j+3);    // 4
        indices.append(j+2);    // 3
        indices.append(j);      // 1
        indices.append(j+1);    // 2

        // we are not last:
        if( i+1<points.size() ){
            indices.append(indices.back());
        }

        ui->openGLWidget->scene()->addItem( VIC::makeVIC(vertices, indices, GL_TRIANGLE_STRIP) );
    }
}


void VolumeViewer::compassToView( QVector3D pos, qreal SIZE, QColor color){

    QVector3D drawColor{static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF()) };
    QVector<VIC::Vertex> vertices;
    QVector<VIC::Index> indices;

    QVector3D scale=ui->openGLWidget->scale();
    qreal sizeX=SIZE/std::fabs(scale.x());
    //qreal sizeY=SIZE/std::fabs(scale.y());
    qreal sizeZ=SIZE/std::fabs(scale.z());

    sizeX/=5;   // arrow narrower than long

    vertices.append( VIC::Vertex{ QVector3D( pos.x(), pos.y(), pos.z()+sizeZ/2), drawColor} );
    vertices.append( VIC::Vertex{ QVector3D( pos.x()-sizeX/2, pos.y(), pos.z()-sizeZ/2), drawColor} );
    vertices.append( VIC::Vertex{ QVector3D( pos.x()+sizeX/2, pos.y(), pos.z()-sizeZ/2), drawColor} );

    indices.append(0);
    indices.append(1);
    indices.append(2);

    ui->openGLWidget->scene()->addItem( VIC::makeVIC(vertices, indices, GL_TRIANGLES) );
}


// x and z scales should be equal
// y of point 1 and point 2 are assumed to be equal, text will be drawn parallel to the y axxis
void VolumeViewer::textToView( QVector3D pos1, QVector3D pos2, QString text, Qt::Alignment align ){

    const qreal relativeSize=m_labelSize;
    const QColor color=m_labelColor;

    QFont font("Helvetica [Cronyx]", 16);
    QFontMetrics metrics(font);

    QRect rect = metrics.tightBoundingRect(text);
    //std::cout<<"rect: x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl<<std::flush;

    QImage img(rect.x() + rect.width(), rect.height(), QImage::Format_RGBA8888);

    img.fill(qRgba(0,0,0,0));   // make image transparent

    QPainter painter(&img);
    painter.setFont(font);
    painter.setPen(color);
    painter.drawText( 0, -rect.y(), text);

    QVector3D scale=ui->openGLWidget->scale();
    qreal horizontalSize=qreal(relativeSize * rect.width() ) / std::fabs(scale.x());
    qreal verticalSize=qreal(relativeSize * rect.height() ) / std::fabs(scale.y());

    // find center
    QVector3D pos=(pos1 + pos2 ) / 2;
    qreal dx=pos2.x() - pos1.x();
    qreal dz=pos2.z() - pos1.z();
    qreal d= std::sqrt(dx*dx + dz*dz);
    qreal ux=dx/d;
    qreal uz=dz/d;

    // adjust y coordinate to alignment
    // vertically default alignment is bottom, i.e. text is above pos
    qreal y=pos.y()-verticalSize;
    if( align & Qt::AlignVCenter ){
        y+=verticalSize/2;
    }
    else if( align & Qt::AlignTop ){
        y+= verticalSize;
    }
    pos.setY(y);

    qreal x1=0,z1=0;
    qreal x2=0,z2=0;
    // adjust horizontal coords (x,z) to alignment
    if( align & Qt::AlignHCenter){
        x1=pos.x()-ux*horizontalSize/2;
        z1=pos.z()-uz*horizontalSize/2;
        x2=pos.x() + ux*horizontalSize/2;
        z2=pos.z()+uz*horizontalSize/2;
    }
    else if( align & Qt::AlignLeft){
        x1=pos2.x();
        z1=pos2.z();
        x2=pos2.x() + ux*horizontalSize;
        z2=pos2.z() + uz*horizontalSize;
    }
    else if( align & Qt::AlignRight){
        x1=pos1.x() - ux*horizontalSize;
        z1=pos1.z() - uz*horizontalSize;
        x2=pos1.x();
        z2=pos1.z();
    }

    QVector<VIT::Vertex> vertices{
        {QVector3D( x1, pos.y(), z1), QVector2D( 0, 0)},   // top left
        {QVector3D( x2, pos.y(), z2 ), QVector2D( 1, 0)},   // top right
        {QVector3D( x2,  pos.y()+verticalSize, z2 ), QVector2D( 1, 1)},    // bottom right
        {QVector3D( x1, pos.y()+verticalSize, z1 ), QVector2D( 0, 1)}   // bottom left
    };

    QVector<VIT::Index> indices{
         0,  1,  2,  3
    };

    ui->openGLWidget->scene()->addItem(VIT::makeVIT(vertices, indices, GL_QUADS, img ) );
}


void VolumeViewer::initialVolumeDisplay(){

    m_sliceModel->clear();

    if( m_volumes.empty()) return;
    QString name=m_volumes.keys().first();
    Grid3DBounds bounds=m_volumes.value(name)->bounds();

    setDimensions( VolumeDimensions(bounds.inline1(), bounds.inline2(), bounds.crossline1(), bounds.crossline2(),
                                    static_cast<int>(1000*bounds.ft()), static_cast<int>(1000*bounds.lt()) ) );

    int iline=(bounds.inline1()+bounds.inline2())/2;
    m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{name, SliceType::INLINE, iline});

    int xline=(bounds.crossline1()+bounds.crossline2())/2;
    m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{name, SliceType::CROSSLINE, xline});

    int msec=static_cast<int>(1000*(bounds.ft() + bounds.lt() )/2);
    m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{name, SliceType::TIME, msec});

    defaultPositionAndScale();
    ui->openGLWidget->setRotation(QVector3D(0,0,0));

    refreshView();

   // ui->openGLWidget->setCenter(QVector3D(xline, sample, iline) );

}


QString VolumeViewer::selectVolume(){

    if( m_volumes.empty()) return QString();

    if( m_volumes.size()==1){
        return m_volumes.keys().first();
    }
    else{
        bool ok=false;
        QString volume=QInputDialog::getItem(this, tr("Select Volume"), tr("Volume"), m_volumes.keys(), 0, false, &ok);
        if( volume.isNull() || !ok ) return QString();
        else return volume;
    }

}


void VolumeViewer::on_action_Volume_Colortable_triggered()
{
    QString volume=selectVolume();
    if( volume.isEmpty() ) return;

    ColorTable* colorTable=m_colorTables.value(volume, nullptr);
    if(!colorTable) return;

    QVector<QRgb> oldColors=colorTable->colors();

    ColorTableDialog colorTableDialog( oldColors );
    colorTableDialog.setWindowTitle(QString("Colortable %1").arg(volume));

    if( colorTableDialog.exec()==QDialog::Accepted ){
        colorTable->setColors( colorTableDialog.colors());
    }else{
        colorTable->setColors( oldColors );
    }

}

void VolumeViewer::on_actionVolume_Range_triggered()
{
    QString volume=selectVolume();
    if( volume.isEmpty() ) return;

    std::shared_ptr<Grid3D<float>> pvolume=m_volumes.value(volume, nullptr);
    if(!pvolume) return;

    ColorTable* colorTable=m_colorTables.value(volume, nullptr);
    if(!colorTable) return;

    HistogramRangeSelectionDialog* displayRangeDialog=m_displayRangeDialogs.value(volume, nullptr);

    if(!displayRangeDialog){

        displayRangeDialog=new HistogramRangeSelectionDialog(this);
        displayRangeDialog->setHistogram(createHistogram( std::begin(*pvolume), std::end(*pvolume),
                                                          pvolume->NULL_VALUE, 100 ));
        displayRangeDialog->setDefaultRange( pvolume->valueRange());
        displayRangeDialog->setColorTable( colorTable );   // all updating through colortable

        displayRangeDialog->setWindowTitle(QString("Range %1").arg(volume) );

        m_displayRangeDialogs.insert(volume, displayRangeDialog );
    }


    displayRangeDialog->show();

}

void VolumeViewer::defaultPositionAndScale(){

    if( !m_dimensions.isValid() ) return;

    QRect br_ilxl( m_dimensions.inline1, m_dimensions.crossline1,
                   m_dimensions.inline2-m_dimensions.inline1+1, m_dimensions.crossline2 - m_dimensions.crossline1 + 1 );
    br_ilxl=br_ilxl.normalized();
    QRectF br_xy=ilxl_to_xy.mapRect(br_ilxl);
    br_xy=br_xy.normalized();

    // compute scale factor to make lines and times same size
    qreal yfac=qreal(std::max(br_xy.width(), br_xy.height()))/(m_dimensions.msec2 - m_dimensions.msec1 + 1)*1000;   //*1000 sec->msec


    qreal cx=(br_xy.left()+br_xy.right())/2; //br_xy.center().x();
    qreal cz=(br_xy.top()+br_xy.bottom())/2;//br_xy.center().y();
    qreal cy=0.5*(m_dimensions.msec1+m_dimensions.msec2)*0.001;

    qreal sizeX=br_xy.width();
    qreal sizeZ=br_xy.height();
    qreal sizeY=(m_dimensions.msec2-m_dimensions.msec1)*0.001;

    QVector3D dimensions(sizeX, sizeY, sizeZ);

    qreal maxSize=std::max({sizeX, yfac*sizeY, sizeZ } );

    ui->openGLWidget->setCenter(QVector3D(cx,cy,cz));
    ui->openGLWidget->setScale(QVector3D(1., -yfac, -1. ) );     // - make y-axis top to bottom, z-axis points backwards
    ui->openGLWidget->setRotation(QVector3D(0,0,0));
    ui->openGLWidget->setPosition(QVector3D( 0, 0, -2*maxSize ) );  // 45 deg fov
    ui->openGLWidget->setDimensions(dimensions);
}

void VolumeViewer::on_action_Front_triggered()
{

    defaultPositionAndScale();
    ui->openGLWidget->setRotation(QVector3D(0,0,0));
}

void VolumeViewer::on_action_Back_triggered()
{
    defaultPositionAndScale();
    ui->openGLWidget->setRotation(QVector3D(0,180.0,0));
}

void VolumeViewer::on_action_Left_triggered()
{
    defaultPositionAndScale();
    ui->openGLWidget->setRotation(QVector3D(0,90.0,0));
}

void VolumeViewer::on_action_Right_triggered()
{
    defaultPositionAndScale();
    ui->openGLWidget->setRotation(QVector3D(0,270.0,0));
}

void VolumeViewer::on_action_Top_triggered()
{
    defaultPositionAndScale();
    ui->openGLWidget->setRotation(QVector3D(90.0,0,0));
}

void VolumeViewer::on_action_Bottom_triggered()
{
    defaultPositionAndScale();
    ui->openGLWidget->setRotation(QVector3D(270.0,0,0));
}




void VolumeViewer::on_actionSet_Point_Size_triggered()
{
    bool ok=false;

    int s=QInputDialog::getInt( this, tr("Set Point Size"), tr("Point Size:"), m_highlightedPointSize, 10, 1000, 10, &ok );
    if( !ok ) return;

    setHighlightedPointSize(s);
}

/*
void VolumeViewer::on_action_Navigation_Dialog_triggered()
{
    if( !m_navigationDialog ){

        m_navigationDialog=new VolumeNavigationDialog(this);
        m_navigationDialog->setWindowTitle(tr("Volume Navigation"));

        VolumeNavigationDialog* navigation=m_navigationDialog;
        connect( navigation, SIGNAL(positionChanged(QVector3D)), ui->openGLWidget, SLOT(setPosition(QVector3D)) );
        connect(ui->openGLWidget, SIGNAL(positionChanged(QVector3D)), navigation, SLOT(setPosition(QVector3D)) );
        connect( navigation, SIGNAL(rotationChanged(QVector3D)), ui->openGLWidget, SLOT(setRotation(QVector3D)) );
        connect(ui->openGLWidget, SIGNAL(rotationChanged(QVector3D)), navigation, SLOT(setRotation(QVector3D)) );
        connect( navigation, SIGNAL(scaleChanged(QVector3D)), ui->openGLWidget, SLOT(setScale(QVector3D)) );
        connect(ui->openGLWidget, SIGNAL(scaleChanged(QVector3D)), navigation, SLOT(setScale(QVector3D)) );
    }

    m_navigationDialog->show();
}
*/

void VolumeViewer::on_actionHistogram_triggered()
{

    QString volume=selectVolume();
    if( volume.isEmpty() ) return;

    std::shared_ptr<Grid3D<float>> pvolume=m_volumes.value(volume, nullptr);
    if(!pvolume) return;

    ColorTable* colorTable=m_colorTables.value(volume, nullptr);
    if(!colorTable) return;


    QVector<double> data;
    for( auto it=pvolume->values().cbegin(); it!=pvolume->values().cend(); ++it){
        if( *it==pvolume->NULL_VALUE) continue;
        data.push_back(*it);
     }

    HistogramDialog* viewer=new HistogramDialog(this);      // need to make this a parent in order to allow qt to delete this when this is deleted
                                                            // this is important because otherwise the colortable will be deleted before this! CRASH!!!
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1").arg(volume ) );
    viewer->setColorTable(colorTable);        // the colortable must have same parent as viewer, maybe used shared_ptr!!!
    viewer->show();
}



void VolumeViewer::on_actionNavigation_Dialog_triggered()
{

    if( !m_navigationDialog ){

        m_navigationDialog=new VolumeNavigationDialog(this);
        m_navigationDialog->setWindowTitle(tr("Volume Navigation"));

        VolumeNavigationDialog* navigation=m_navigationDialog;
        connect( navigation, SIGNAL(positionChanged(QVector3D)), ui->openGLWidget, SLOT(setPosition(QVector3D)) );
        connect(ui->openGLWidget, SIGNAL(positionChanged(QVector3D)), navigation, SLOT(setPosition(QVector3D)) );
        connect( navigation, SIGNAL(rotationChanged(QVector3D)), ui->openGLWidget, SLOT(setRotation(QVector3D)) );
        connect(ui->openGLWidget, SIGNAL(rotationChanged(QVector3D)), navigation, SLOT(setRotation(QVector3D)) );
        connect( navigation, SIGNAL(scaleChanged(QVector3D)), ui->openGLWidget, SLOT(setScale(QVector3D)) );
        connect(ui->openGLWidget, SIGNAL(scaleChanged(QVector3D)), navigation, SLOT(setScale(QVector3D)) );
    }

    m_navigationDialog->setPosition( ui->openGLWidget->position() );
    m_navigationDialog->setRotation( ui->openGLWidget->rotation() );
    m_navigationDialog->setScale( ui->openGLWidget->scale() );

    m_navigationDialog->show();
}

void VolumeViewer::on_action_Background_Color_triggered()
{
    QColor c = QColorDialog::getColor( ui->openGLWidget->backgroundColor(),
                                       this,
                                       tr("Set Background Color")
                                       );

    if( ! c.isValid() ) return;

    ui->openGLWidget->setBackgroundColor(c);
}

void VolumeViewer::on_actionEdit_Slices_triggered()
{
    if( m_volumes.empty()) return;

    if( !editSlicesDialog ){

        editSlicesDialog = new EditSlicesDialog( this );

        editSlicesDialog->setDimensions(m_dimensions);
        editSlicesDialog->setVolumes(m_volumes.keys());
        editSlicesDialog->setWindowTitle(tr("Edit Slices"));

        editSlicesDialog->setSliceModel(m_sliceModel);

        connect( this, SIGNAL(volumeListChanged(QStringList)), editSlicesDialog, SLOT(setVolumes(QStringList)) );
        connect( this, SIGNAL(dimensionsChanged(VolumeDimensions)), editSlicesDialog, SLOT(setDimensions(VolumeDimensions)) );
    }

    editSlicesDialog->show();
}


void VolumeViewer::on_actionEdit_Horizons_triggered()
{
    if( !editHorizonsDialog ){

        editHorizonsDialog=new EditHorizonsDialog( this );
        editHorizonsDialog->setVolumes(m_volumes.keys());
        editHorizonsDialog->setHorizonModel(m_horizonModel);    // dialog and manager have this as parent
        editHorizonsDialog->setProject( m_project );
        editHorizonsDialog->setWindowTitle(tr("Edit Horizons"));

        connect( this, SIGNAL(volumeListChanged(QStringList)), editHorizonsDialog, SLOT(setVolumes(QStringList)) );
    }

    editHorizonsDialog->show();
}

void VolumeViewer::on_action_Open_Volume_triggered()
{
    if( !m_project )return;

    bool ok=false;
    QString name=QInputDialog::getItem( this, tr("Open Volume"), tr("Select Volume"),
                                        m_project->volumeList(), 0, false, &ok);

    if( !ok || name.isNull() ) return;

    std::shared_ptr<Grid3D<float> > volume=m_project->loadVolume( name);
    if( !volume ){
        QMessageBox::critical(this, tr("Open Volume"), tr("Loading Volume failed!"));
        return;
    }

    addVolume(name, volume);
}



void VolumeViewer::on_actionReset_Highlighted_Points_triggered()
{
    QVector<SelectionPoint> empty;

    setHighlightedPoints(empty);
}

void VolumeViewer::on_actionLimit_Highlighted_Points_triggered()
{
    bool ok=false;

    int max=QInputDialog::getInt(this, tr("Limit Highlighted Points"), tr("Maximum Number:"),
                                 m_maxHighlightedPoints, 0, 1000000, 1000, &ok);

    if( !ok ) return;

    // only applies to next reception, clear highlighted points if present
    m_maxHighlightedPoints=max;
    ui->actionReset_Highlighted_Points->trigger();
}

void VolumeViewer::on_actionSet_Point_Color_triggered()
{
    QColor color=QColorDialog::getColor( m_highlightedPointColor, this, tr("Set Point Color") );

    if( !color.isValid()) return;

    setHighlightedPointColor(color);
}

void VolumeViewer::on_actionSet_Compass_Size_triggered()
{
    bool ok=false;

    int s=QInputDialog::getInt( this, tr("Set Compass Size"), tr("Compass Size:"), m_compassSize, 100, 100000, 100, &ok );
    if( !ok ) return;

    setCompassSize(s);
}

void VolumeViewer::on_actionSet_Compass_Color_triggered()
{
    QColor color=QColorDialog::getColor( m_compassColor, this, tr("Set Compass Color") );

    if( !color.isValid()) return;

    setCompassColor(color);
}

void VolumeViewer::on_actionSet_Outline_Color_triggered()
{
    QColor color=QColorDialog::getColor( m_outlineColor, this, tr("Set Outline Color") );

    if( !color.isValid()) return;

    setOutlineColor(color);
}

void VolumeViewer::on_actionSet_Label_size_triggered()
{
    bool ok=false;

    int s=QInputDialog::getInt( this, tr("Set Label Size"), tr("Label Size:"), m_labelSize, 1, 1000, 1, &ok );
    if( !ok ) return;

    setLabelSize(s);
}

void VolumeViewer::on_actionSet_Label_Color_triggered()
{
    QColor color=QColorDialog::getColor( m_labelColor, this, tr("Set Label Color") );

    if( !color.isValid()) return;

    setLabelColor(color);

}



void VolumeViewer::on_actionSet_Dimensions_triggered()
{
   VolumeDataSelectionDialog dlg;
   dlg.setDimensions(m_dimensions);
   dlg.setWindowTitle(tr("Set Dimensions"));

   if( dlg.exec()==QDialog::Accepted){
       setDimensions(dlg.dimensions());
   }
}

void VolumeViewer::on_action_Close_Volume_triggered()
{
    QString volume=selectVolume();

    if( volume.isEmpty()) return;

    removeVolume(volume);
}
