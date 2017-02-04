#include "volumeviewer.h"
#include "ui_volumeviewer.h"
#include <colortable.h>
#include <colortabledialog.h>
#include <modaldisplayrangedialog.h>
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

VolumeViewer::VolumeViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::VolumeViewer),
    m_colorTable(new ColorTable(this)),
     m_horizonModel(new HorizonModel(this)),
    m_sliceModel(new SliceModel(this))
{
    ui->setupUi(this);

    m_colorTable->setColors(ColorTable::defaultColors());

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(refreshView()) );
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refreshView()) );
    connect( m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(refreshView()) );
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

    ui->menu_Window->addAction( m_volumeColorBarDock->toggleViewAction());
    ui->menu_Window->addAction( m_navigationControlsDock->toggleViewAction());
    ui->menu_Window->addSeparator();
    ui->menu_Window->addAction( ui->toolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->toolBar_View->toggleViewAction());
}

void VolumeViewer::createDockWidgets(){

    m_volumeColorBarDock = new QDockWidget(tr("Volume Colorbar"), this);
    m_volumeColorBarDock->setAllowedAreas(Qt::RightDockWidgetArea);
    m_volumeColorBarDock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar |
                                      QDockWidget::DockWidgetClosable |
                                      QDockWidget::DockWidgetMovable |
                                      QDockWidget::DockWidgetFloatable);
    m_volumeColorBarWidget=new ColorBarWidget(m_volumeColorBarDock);
    m_volumeColorBarWidget->setScaleAlignment(ColorBarWidget::SCALE_LEFT);
    m_volumeColorBarDock->setContentsMargins(10, 5, 10, 5);
    m_volumeColorBarDock->setWidget(m_volumeColorBarWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_volumeColorBarDock);
    m_volumeColorBarWidget->setColorTable( m_colorTable );
    //m_volumeColorBarWidget->setLabel("Volume Amplitudes");
    m_volumeColorBarDock->close();

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

void VolumeViewer::setProject(std::shared_ptr<AVOProject> p){

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

void VolumeViewer::setVolume( std::shared_ptr<Grid3D<float> > volume){

    if( volume==m_volume) return;

    std::shared_ptr<Grid3D<float>> old = m_volume;  // keep this for geometry comparison

    m_volume=volume;

    if( m_volume ){

        if( !isColorMappingLocked() ){
            m_colorTable->setRange(m_volume->valueRange());
        }

        if( !old || !volume || old->bounds() !=volume->bounds()){
            initialVolumeDisplay();
        }
        else{
            refreshView();
        }
    }
}

void VolumeViewer::setHighlightedPoints(QVector<SelectionPoint> rpoints){

    m_highlightedPoints=rpoints;

    refreshView();
}

void VolumeViewer::setColorMappingLocked(bool on){
    m_colorMappingLock=on;
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

    Q_ASSERT( m_volume);

    ui->openGLWidget->makeCurrent(); // need this to access currect VBOs

    RenderScene* scene=view->scene();
    //if( !scene ) return;
    Q_ASSERT(scene);

    scene->clear();

    Grid3DBounds bounds=m_volume->bounds();
    QVector<int> ilines;
    QVector<int> xlines;

    // volume outline
    if( ui->actionShow_Volume_Outline->isChecked()){
        outlineToView( bounds, m_outlineColor);

        // add min/max inline and crossline to labels if outline is drawn
        ilines<<bounds.inline1();
        ilines<<bounds.inline2();
        xlines<<bounds.crossline1();
        xlines<<bounds.crossline2();
    }

    // slices
    foreach( QString name, m_sliceModel->names()){
        SliceDef slice=m_sliceModel->slice(name);
        sliceToView(slice);
        // add slice to label list
        if( slice.type==SliceType::INLINE) ilines<<slice.value;
        else if( slice.type==SliceType::CROSSLINE) xlines<<slice.value;
    }



    // outline + slice labels
    if( ui->actionShow_Labels->isChecked()){

        int topMS=1000*bounds.ft();
        int bottomMS=1000*bounds.lt();

        // inlines
        foreach( int iline, ilines){

            QPointF xy1=ilxl_to_xy.map( QPoint(iline, bounds.crossline1() ) );
            QPointF xy2=ilxl_to_xy.map( QPoint(iline, bounds.crossline2() ) );
            QString strTop=QString("iline %1 @%2ms").arg(iline).arg(topMS);
            QString strBottom=QString("iline %1 @%2ms").arg(iline).arg(bottomMS);
            textToView( QVector3D( xy1.x(), bounds.ft(), xy1.y()), QVector3D( xy2.x(), bounds.ft(), xy2.y()),
                        strTop, Qt::AlignBottom);
            textToView( QVector3D( xy2.x(), bounds.lt(), xy2.y()), QVector3D( xy1.x(), bounds.lt(), xy1.y()),
                        strBottom, Qt::AlignTop);

        }


        // crosslines
        foreach( int xline, xlines){

            QPointF xy1=ilxl_to_xy.map( QPoint(bounds.inline1(), xline ) );
            QPointF xy2=ilxl_to_xy.map( QPoint(bounds.inline2(), xline ) );
            QString strTop=QString("xline %1 @%2ms").arg(xline).arg(topMS);
            QString strBottom=QString("xline %1 @%2ms").arg(xline).arg(bottomMS);
            textToView( QVector3D( xy1.x(), bounds.ft(), xy1.y()), QVector3D( xy2.x(), bounds.ft(), xy2.y()),
                        strTop, Qt::AlignBottom);
            textToView( QVector3D( xy2.x(), bounds.lt(), xy2.y()), QVector3D( xy1.x(), bounds.lt(), xy1.y()),
                        strBottom, Qt::AlignTop);

        }

    }


    // horizons
    foreach( QString hname, m_horizonModel->names() ){

        HorizonDef hdef=m_horizonModel->item(hname);
        Grid2D<float>* h = hdef.horizon.get();

        if( h ){

            if( hdef.useColor ){
                horizonToView(h, hdef.color, hdef.delay);
            }
            else{
                horizonToView(h, hdef.delay );
            }
        }
    }

    // highlighted points
    pointsToView( m_highlightedPoints, m_highlightedPointColor, m_highlightedPointSize );


    // compasses / north indicators
    if( ui->actionShow_Compass->isChecked()){
        // draw compasses on every corner at top and bottom of outline cube
        Grid3DBounds bounds=m_volume->bounds();
        QVector<QPoint> compassPoints{ QPoint{ bounds.inline1(), bounds.crossline1() },
                                       QPoint{ bounds.inline1(), bounds.crossline2() },
                                       QPoint{ bounds.inline2(), bounds.crossline1() },
                                       QPoint{ bounds.inline2(), bounds.crossline2() } };
        foreach( QPoint ilxl, compassPoints){
            QPointF xy=ilxl_to_xy.map(ilxl);
            compassToView( QVector3D{static_cast<float>(xy.x()), static_cast<float>(bounds.ft()), static_cast<float>(xy.y())},
                           m_compassSize, m_compassColor );
            compassToView( QVector3D{static_cast<float>(xy.x()), static_cast<float>(bounds.lt()), static_cast<float>(xy.y())},
                           m_compassSize, m_compassColor );
        }
    }

    ui->openGLWidget->update();
}

void VolumeViewer::receivePoint( SelectionPoint point, int code ){

    Q_ASSERT(m_volume);

    switch( code ){

    case PointCode::VIEWER_POINT_SELECTED:{

       if( point.iline!=SelectionPoint::NO_LINE){
            m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{SliceType::INLINE, point.iline} );
        }
        if( point.xline!=SelectionPoint::NO_LINE){
            m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{SliceType::CROSSLINE, point.xline} );
        }
        break;
    }

    case PointCode::VIEWER_TIME_SELECTED:{
        int msec=static_cast<int>(1000*point.time);
        m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{SliceType::TIME, msec} );
        break;
    }
    default:{               // nop

        break;
    }

    }

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

void VolumeViewer::outlineToView(Grid3DBounds bounds, QColor color){

    QVector3D vcolor{ static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF() ) };

    QPoint ILXL1(bounds.inline1(), bounds.crossline1());
    QPoint ILXL2(bounds.inline1(), bounds.crossline2());
    QPoint ILXL3(bounds.inline2(), bounds.crossline1());
    QPoint ILXL4(bounds.inline2(), bounds.crossline2());

    QPointF XY1=ilxl_to_xy.map(ILXL1);
    QPointF XY2=ilxl_to_xy.map(ILXL2);
    QPointF XY3=ilxl_to_xy.map(ILXL3);
    QPointF XY4=ilxl_to_xy.map(ILXL4);

    QVector<VIC::Vertex> vertices{
        {QVector3D( XY1.x(), bounds.ft(), XY1.y()), vcolor },
        {QVector3D( XY2.x(), bounds.ft(), XY2.y()), vcolor },
        {QVector3D( XY4.x(), bounds.ft(), XY4.y()), vcolor },
        {QVector3D( XY3.x(), bounds.ft(), XY3.y()), vcolor },
        {QVector3D( XY1.x(), bounds.lt(), XY1.y()), vcolor },
        {QVector3D( XY2.x(), bounds.lt(), XY2.y()), vcolor },
        {QVector3D( XY4.x(), bounds.lt(), XY4.y()), vcolor },
        {QVector3D( XY3.x(), bounds.lt(), XY3.y()), vcolor }
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
    case SliceType::INLINE: inlineSliceToView(def.value);break;
    case SliceType::CROSSLINE: crosslineSliceToView(def.value); break;
    case SliceType::TIME: timeSliceToView(def.value); break;
    default: qFatal("Invalid SliceType!!!");
    }
}

void VolumeViewer::inlineSliceToView( int iline ){

    Q_ASSERT( m_volume);

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds bounds=volume.bounds();

    if( iline<bounds.inline1() || iline>bounds.inline2()) return;

    QImage img(bounds.crosslineCount(), bounds.sampleCount(), QImage::Format_RGBA8888);// QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){

            Grid3D<float>::value_type value=volume(iline, bounds.crossline1()+i, j);
            ColorTable::color_type color=(value!=m_volume->NULL_VALUE) ? m_colorTable->map(value) : qRgba(0,0,0,0);
            img.setPixel(i,j, color );

            //img.setPixel(i,j,m_colorTable->map(volume(iline, bounds.crossline1()+i, j) ) );
        }
    }

    QPointF xy1=ilxl_to_xy.map(QPoint(iline, bounds.crossline1()));
    QPointF xy2=ilxl_to_xy.map(QPoint(iline, bounds.crossline2()));

    QVector<VIT::Vertex> vertices{
        {QVector3D( xy1.x(), bounds.ft(), xy1.y()), QVector2D( 0, 0)},   // top left
        {QVector3D( xy2.x(), bounds.ft(), xy2.y()), QVector2D( 1, 0)},   // top right
        {QVector3D( xy1.x(), bounds.lt(), xy1.y()), QVector2D( 0, 1)},   // bottom left
        {QVector3D( xy2.x(), bounds.lt(), xy2.y()), QVector2D( 1, 1)}    // bottom right
    };

    QVector<VIT::Index> indices{
         0,  1,  2,  3
    };

    ui->openGLWidget->scene()->addItem(VIT::makeVIT(vertices, indices, GL_TRIANGLE_STRIP, img) );
}

void VolumeViewer::crosslineSliceToView( int xline ){

    Q_ASSERT( m_volume);

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds bounds=volume.bounds();

    if( xline<bounds.crossline1() || xline>bounds.crossline2()) return;

    QImage img(bounds.inlineCount(), bounds.sampleCount(), QImage::Format_RGBA8888);// QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){

            Grid3D<float>::value_type value=volume(bounds.inline1()+i, xline, j);
            ColorTable::color_type color=(value!=m_volume->NULL_VALUE) ? m_colorTable->map(value) : qRgba(0,0,0,0);
            img.setPixel(i,j, color );

            //img.setPixel(i,j,m_colorTable->map(volume(bounds.inline1()+i, xline, j) ) );
        }
    }

    QPointF xy1=ilxl_to_xy.map(QPoint(bounds.inline1(), xline));
    QPointF xy2=ilxl_to_xy.map(QPoint(bounds.inline2(), xline));

    QVector<VIT::Vertex> vertices{
        {QVector3D( xy1.x(), bounds.ft(), xy1.y()), QVector2D( 0, 0)},   // top left
        {QVector3D( xy2.x(), bounds.ft(), xy2.y()), QVector2D( 1, 0)},   // top right
        {QVector3D( xy1.x(), bounds.lt(), xy1.y()), QVector2D( 0, 1)},   // bottom left
        {QVector3D( xy2.x(), bounds.lt(), xy2.y()), QVector2D( 1, 1)}    // bottom right
    };

    QVector<VIT::Index> indices{
         0,  1,  2,  3
    };

    ui->openGLWidget->scene()->addItem(VIT::makeVIT(vertices, indices, GL_TRIANGLE_STRIP, img) );

}

void VolumeViewer::timeSliceToView( int msec ){

    Q_ASSERT( m_volume);

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds bounds=volume.bounds();

    qreal time=0.001*msec;

    if( time<bounds.ft() || time>bounds.lt()  ) return;

    QImage img(bounds.inlineCount(), bounds.crosslineCount(), QImage::Format_RGBA8888); //QImage::Format_RGB32 );
    for( int il=bounds.inline1(); il<=bounds.inline2(); il++){
        for( int xl=bounds.crossline1(); xl<=bounds.crossline2(); xl++){
            int xi=il-bounds.inline1();
            int yi=xl-bounds.crossline1();

            Grid3D<float>::value_type value=volume.value(il, xl, time);
            ColorTable::color_type color=(value!=m_volume->NULL_VALUE) ? m_colorTable->map(value) : qRgba(0,0,0,0);
            img.setPixel(xi, yi, color );

            //img.setPixel( xi, yi, m_colorTable->map(volume(il, xl, sample) ) );
        }
    }

    QPoint ILXL1(bounds.inline1(), bounds.crossline1());
    QPoint ILXL2(bounds.inline1(), bounds.crossline2());
    QPoint ILXL3(bounds.inline2(), bounds.crossline1());
    QPoint ILXL4(bounds.inline2(), bounds.crossline2());

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
void VolumeViewer::horizonToView(Grid2D<float>* hrz, int delayMSec){

    Q_ASSERT( m_volume );
    Q_ASSERT( hrz );

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
                qreal t = 0.001*(delayMSec+tms);        // convert time to seconds

                QVector3D color = nullColor;

                float value = m_volume->value(iline, xline, t);

                if( value != m_volume->NULL_VALUE ){

                    QColor c = m_colorTable->map(value);
                    color = QVector3D{static_cast<float>(c.redF()), static_cast<float>(c.greenF()), static_cast<float>(c.blueF()) };
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


void VolumeViewer::horizonToView(Grid2D<float>* hrz, QColor hcolor, int delayMSec){

    Q_ASSERT( m_volume );
    Q_ASSERT( hrz );

    //Grid3D<float>& volume=*m_volume;
    //Grid3DBounds vbounds=volume.bounds();

    Grid2DBounds hbounds=hrz->bounds();

    auto range=valueRange(*hrz);
    QVector3D nullColor{0,0,0};
    QVector3D baseColor{static_cast<float>(hcolor.redF()), static_cast<float>(hcolor.greenF()), static_cast<float>(hcolor.blueF()) };
    QVector<VIC::Vertex> vertices;

    for( int iline=hbounds.i1(); iline<=hbounds.i2(); iline++){

        for( int xline=hbounds.j1(); xline<=hbounds.j2(); xline++){

            float tms=(*hrz)(iline, xline);  // horizon is in millis
            QPointF xy=ilxl_to_xy.map(QPoint(iline, xline));

            if(tms!=hrz->NULL_VALUE){
                // scale colour according to relative depth, deeper->darker
                float x=(tms-range.first)/(range.second-range.first);
                qreal t = 0.001*(delayMSec+tms);        // convert time to seconds

                QVector3D color = nullColor;

                float value = m_volume->value(iline, xline, t);

                if( value != m_volume->NULL_VALUE ){

                    color = baseColor*(2.-x)/2;
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

    Q_ASSERT( m_volume );

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
void VolumeViewer::textToView( QVector3D pos1, QVector3D pos2, QString text, Qt::Alignment valign ){

    const qreal relativeSize=m_labelSize;
    const QColor color=m_labelColor;

    QFont font("Helvetica [Cronyx]", 16);
    QFontMetrics metrics(font);

    QRect rect = metrics.tightBoundingRect(text);
    //std::cout<<"rect: x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl;

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

    // adjust y coordinate to alignment
    // vertically default alignment is bottom, i.e. text is above pos
    qreal y=pos.y()-verticalSize;
    if( valign & Qt::AlignVCenter ){
        y+=verticalSize/2;
    }
    else if( valign & Qt::AlignTop ){
        y+= verticalSize;
    }
    pos.setY(y);

    qreal dx=pos2.x() - pos1.x();
    qreal dz=pos2.z() - pos1.z();
    qreal d= std::sqrt(dx*dx + dz*dz);
    qreal ux=dx/d;
    qreal uz=dz/d;

    QVector<VIT::Vertex> vertices{
        {QVector3D( pos.x()-ux*horizontalSize/2, pos.y(), pos.z()-uz*horizontalSize/2), QVector2D( 0, 0)},   // top left
        {QVector3D( pos.x() + ux*horizontalSize/2, pos.y(), pos.z()+uz*horizontalSize/2 ), QVector2D( 1, 0)},   // top right
        {QVector3D( pos.x() + ux*horizontalSize/2,  pos.y()+verticalSize, pos.z()+uz*horizontalSize/2 ), QVector2D( 1, 1)},    // bottom right
        {QVector3D( pos.x()-ux*horizontalSize/2, pos.y()+verticalSize, pos.z()-uz*horizontalSize/2 ), QVector2D( 0, 1)}   // bottom left
    };

    QVector<VIT::Index> indices{
         0,  1,  2,  3
    };

    ui->openGLWidget->scene()->addItem(VIT::makeVIT(vertices, indices, GL_QUADS, img ) );
}

/*
 * // x and z scales should be equal
void VolumeViewer::textToView( QVector3D pos, QString text, Qt::Alignment align ){

    std::cout<<"text= "<<text.toStdString()<<std::endl;

    QFont font("Helvetica [Cronyx]", 20);
    QFontMetrics metrics(font);

    QRect rect = metrics.tightBoundingRect(text);

    std::cout<<"rect: x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl;

    QImage img(rect.x() + rect.width(), rect.height(), QImage::Format_RGBA8888);

    img.fill(qRgba(0,0,0,0));
    //QPixmap pixmap=QPixmap::fromImage(img);

    QPainter painter(&img);
    //painter.begin(&pixmap);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText( 0, -rect.y(), text);

    const qreal fac = 20;

    QVector3D scale=ui->openGLWidget->scale();
    qreal sizeX=qreal(fac * rect.width())/std::fabs(scale.x());
    qreal sizeY=qreal(fac * rect.height())/std::fabs(scale.y());


    // horizontally first charecter starts at pos , default is align left
    qreal x=pos.x();
    if( align & Qt::AlignHCenter ){
        x-= sizeX/2;
    }
    else if( align & Qt::AlignRight ){
        x-= sizeX;
    }
    pos.setX(x);

    // vertically default alignment is bottom, i.e. text is above pos
    qreal y=pos.y()-sizeY;
    if( align & Qt::AlignVCenter ){
        y+=sizeY/2;
    }
    else if( align & Qt::AlignTop ){
        y+= sizeY;
    }
    pos.setY(y);

    QVector<VIT::Vertex> vertices{
        {QVector3D( pos.x(), pos.y(), pos.z()), QVector2D( 0, 0)},   // top left
        {QVector3D( pos.x() + sizeX, pos.y(), pos.z() ), QVector2D( 1, 0)},   // top right
        {QVector3D( pos.x()+sizeX,  pos.y()+sizeY, pos.z() ), QVector2D( 1, 1)},    // bottom right
        {QVector3D( pos.x(), pos.y()+sizeY, pos.z() ), QVector2D( 0, 1)}   // bottom left
    };

    QVector<VIT::Index> indices{
         0,  1,  2,  3
    };

    ui->openGLWidget->scene()->addItem(VIT::makeVIT(vertices, indices, GL_QUADS, img ) );

}
*/

void VolumeViewer::initialVolumeDisplay(){

    Q_ASSERT( m_volume );

    m_sliceModel->clear();

    Grid3DBounds bounds=m_volume->bounds();

    int iline=(bounds.inline1()+bounds.inline2())/2;
    m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{SliceType::INLINE, iline});

    int xline=(bounds.crossline1()+bounds.crossline2())/2;
    m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{SliceType::CROSSLINE, xline});

    int msec=static_cast<int>(1000*(bounds.ft() + bounds.lt() )/2);
    m_sliceModel->addSlice( m_sliceModel->generateName(), SliceDef{SliceType::TIME, msec});

   // ui->openGLWidget->setCenter(QVector3D(xline, sample, iline) );

    defaultPositionAndScale();
    ui->openGLWidget->setRotation(QVector3D(0,0,0));

    refreshView();
}



void VolumeViewer::on_action_Volume_Colortable_triggered()
{
    QVector<QRgb> oldColors=m_colorTable->colors();

    ColorTableDialog colorTableDialog( oldColors );


    if( colorTableDialog.exec()==QDialog::Accepted ){
        m_colorTable->setColors( colorTableDialog.colors());
    }else{
        m_colorTable->setColors( oldColors );
    }

}

void VolumeViewer::on_actionVolume_Range_triggered()
{

    if(!displayRangeDialog){

        displayRangeDialog=new DisplayRangeDialog(this);

        displayRangeDialog->setPower( m_colorTable->power());
        displayRangeDialog->setRange( m_colorTable->range());
        displayRangeDialog->setLocked( isColorMappingLocked() );
        connect( displayRangeDialog, SIGNAL(rangeChanged(std::pair<double,double>)),
                 m_colorTable, SLOT(setRange(std::pair<double,double>)) );
        connect( displayRangeDialog, SIGNAL(powerChanged(double)),
                 m_colorTable, SLOT( setPower(double)) );
        connect( displayRangeDialog, SIGNAL(lockedChanged(bool)),
                 this, SLOT(setColorMappingLocked(bool)) );
        connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)),
                 displayRangeDialog, SLOT(setRange(std::pair<double,double>)) );
        connect( m_colorTable, SIGNAL(powerChanged(double)),
                 displayRangeDialog, SLOT(setPower(double)) );
    }

    displayRangeDialog->show();

/*
    ModalDisplayRangeDialog dlg;
    dlg.setWindowTitle(tr("Edit Volume Range"));
    dlg.setMinimum(m_colorTable->range().first);
    dlg.setMaximum(m_colorTable->range().second);
    dlg.setPower(m_colorTable->power());

    if( dlg.exec()==QDialog::Accepted){

        m_colorTable->setRange(dlg.minimum(), dlg.maximum());
        m_colorTable->setPower(dlg.power());
        refreshView();
    }
    */

}

/*
void VolumeViewer::on_action_Add_Slice_triggered()
{

    AddSliceDialog dlg;
    dlg.setWindowTitle(tr("Add Slice"));

    if( dlg.exec()==QDialog::Accepted){

        SliceDef def=dlg.sliceDef();

        m_slices.append(def);
        refreshView();
    }
}
*/
/*
void VolumeViewer::on_action_List_Slices_triggered()
{
    QStringList slices;

    foreach( SliceDef slice, m_slices){

        QString str=QString("%1 %2").arg(toQString(slice.type)).arg(slice.value);
        slices.append(str);
    }

    bool ok=false;

    QString slice=QInputDialog::getItem(this, tr("Remove Slice"), tr("Select Slice"), slices, 0, false, &ok);
    if( slice.isEmpty() || !ok ) return;

    int idx=slices.indexOf(slice);

    removeSlice( m_slices.at(idx) );
}
*/
/*
void VolumeViewer::addHorizon(){

    static const QVector<QColor> HorizonColors{Qt::darkRed, Qt::darkGreen, Qt::darkBlue,
                Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow,
                Qt::red, Qt::green, Qt::blue,
                Qt::cyan, Qt::magenta, Qt::yellow};


    if( !m_project ) return;

    QStringList notLoaded;
    foreach( QString name, m_project->gridList(GridType::Horizon) ){

        if( ! m_horizonManager->contains( name )) notLoaded.append(name);
    }

    // nothing left to load
    if( notLoaded.isEmpty() ) return;

    bool ok=false;
    QString gridName=QInputDialog::getItem(this, "Open Horizon", "Please select a horizon:",
                                           notLoaded, 0, false, &ok);
    if( !gridName.isEmpty() && ok ){


        if( m_horizonManager->contains( gridName) ){
                QMessageBox::information(this, tr("Add Horizon"), QString("Horizon \"%1\" is already loaded!").arg(gridName));
                return;
        }

        std::shared_ptr<Grid2D<float> > grid=m_project->loadGrid(GridType::Horizon, gridName );
        if( !grid ){
            QMessageBox::critical(this, tr("Add Horizon"), QString("Loading horizon \"%1\" failed!"). arg(gridName) );
            return;
        }

        HorizonParameters def{ false, HorizonColors.at(m_horizonManager->size()%HorizonColors.size()) };

        m_horizonManager->add(gridName, def, grid );
    }

}
*/

void VolumeViewer::defaultPositionAndScale(){

    Q_ASSERT(m_volume);

    Grid3DBounds bounds=m_volume->bounds();

    QRect br_ilxl( bounds.inline1(), bounds.crossline1(), bounds.inlineCount(), bounds.crosslineCount() );
    br_ilxl=br_ilxl.normalized();
    QRectF br_xy=ilxl_to_xy.mapRect(br_ilxl);
    br_xy=br_xy.normalized();

    // compute scale factor to make lines and times same size
    qreal yfac=qreal(std::max(br_xy.width(), br_xy.height()))/(bounds.lt()-bounds.ft());


    qreal cx=(br_xy.left()+br_xy.right())/2; //br_xy.center().x();
    qreal cz=(br_xy.top()+br_xy.bottom())/2;//br_xy.center().y();
    qreal cy=0.5*(bounds.ft()+bounds.lt());

    qreal sizeX=br_xy.width();
    qreal sizeZ=br_xy.height();
    qreal sizeY=(bounds.lt()-bounds.ft());

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
    if( ! m_volume ) return;

    QVector<double> data;
    for( auto it=m_volume->values().cbegin(); it!=m_volume->values().cend(); ++it){
        if( *it==m_volume->NULL_VALUE) continue;
        data.push_back(*it);
     }

    HistogramDialog* viewer=new HistogramDialog(this);      // need to make this a parent in order to allow qt to delete this when this is deleted
                                                            // this is important because otherwise the colortable will be deleted before this! CRASH!!!
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1").arg(windowTitle() ) );
    viewer->setColorTable(m_colorTable);        // the colortable must have same parent as viewer, maybe used shared_ptr!!!
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
    if( !editSlicesDialog ){

        editSlicesDialog = new EditSlicesDialog( this );

        editSlicesDialog->setBounds( m_volume->bounds() );
        editSlicesDialog->setWindowTitle(tr("Edit Slices"));

        editSlicesDialog->setSliceModel(m_sliceModel);
    }

    editSlicesDialog->show();
}


void VolumeViewer::on_actionEdit_Horizons_triggered()
{
    if( !editHorizonsDialog ){

        editHorizonsDialog=new EditHorizonsDialog( this );

        editHorizonsDialog->setHorizonModel(m_horizonModel);    // dialog and manager have this as parent

        editHorizonsDialog->setProject( m_project );

        editHorizonsDialog->setWindowTitle(tr("Edit Horizons"));
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

    setVolume(volume);

    setWindowTitle( QString("Volume Viewer - %1").arg(name ));
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
