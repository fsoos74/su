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
    m_overlayColorTable(new ColorTable(this)),
     m_horizonModel(new HorizonModel(this)),
    m_sliceModel(new SliceModel(this))
{
    ui->setupUi(this);

    m_colorTable->setColors(ColorTable::defaultColors());
    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(refreshView()) );
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refreshView()) );
    connect( m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(refreshView()) );

    m_overlayColorTable->setColors(ColorTable::defaultColors());
    connect( m_overlayColorTable, SIGNAL(colorsChanged()), this, SLOT(refreshView()) );
    connect( m_overlayColorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(refreshView()) );
    connect( m_overlayColorTable, SIGNAL(powerChanged(double)), this, SLOT(refreshView()) );

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
    ui->menu_Window->addAction( m_overlayColorBarDock->toggleViewAction());
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
    m_volumeColorBarWidget->setMinimumHeight(200);
    m_volumeColorBarWidget->setScaleAlignment(ColorBarWidget::SCALE_LEFT);
    m_volumeColorBarDock->setContentsMargins(10, 5, 10, 5);
    m_volumeColorBarDock->setWidget(m_volumeColorBarWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_volumeColorBarDock);
    m_volumeColorBarWidget->setColorTable( m_colorTable );
    //m_volumeColorBarWidget->setLabel("Volume Amplitudes");
    m_volumeColorBarDock->close();

    m_overlayColorBarDock = new QDockWidget(tr("Overlay Colorbar"), this);
    m_overlayColorBarDock->setAllowedAreas(Qt::RightDockWidgetArea);
    m_overlayColorBarDock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar |
                                      QDockWidget::DockWidgetClosable |
                                      QDockWidget::DockWidgetMovable |
                                      QDockWidget::DockWidgetFloatable);
    m_overlayColorBarWidget=new ColorBarWidget(m_overlayColorBarDock);
     m_overlayColorBarWidget->setMinimumHeight(200);
    m_overlayColorBarWidget->setScaleAlignment(ColorBarWidget::SCALE_LEFT);
    m_overlayColorBarDock->setContentsMargins(10, 5, 10, 5);
    m_overlayColorBarDock->setWidget(m_overlayColorBarWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_overlayColorBarDock);
    m_overlayColorBarWidget->setColorTable( m_overlayColorTable );
    //m_volumeColorBarWidget->setLabel("Volume Amplitudes");
    m_overlayColorBarDock->close();

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

void VolumeViewer::setVolume( std::shared_ptr<Grid3D<float> > volume){

    if( volume==m_volume) return;

    std::shared_ptr<Grid3D<float>> old = m_volume;  // keep this for geometry comparison

    m_volume=volume;

    if( m_volume ){

        if( displayRangeDialog ){
            displayRangeDialog->setHistogram(
                createHistogram( std::begin(*m_volume), std::end(*m_volume), m_volume->NULL_VALUE, 100 )
                );
            //displayRangeDialog->setHistogram(createHistogram(m_grid.get()));
            displayRangeDialog->setRange(m_volume->valueRange());
        }

        m_colorTable->setRange(m_volume->valueRange());

        if( !old || !volume || old->bounds() !=volume->bounds()){
            initialVolumeDisplay();
        }
        else{
            refreshView();
        }
    }
}

void VolumeViewer::setVolumeName(QString name){

    if( name==m_volumeName) return;

    m_volumeName=name;

    m_volumeColorBarDock->setWindowTitle(QString("%1 - Colortable").arg(name));

    setWindowTitle( QString("Volume Viewer - %1").arg(name ));
}

void VolumeViewer::setOverlayVolume( std::shared_ptr<Grid3D<float> > volume){

    if( volume==m_overlayVolume) return;

    m_overlayVolume=volume;

    if( m_overlayVolume ){

        //if( !isColorMappingLocked() ){
            m_overlayColorTable->setRange(m_overlayVolume->valueRange());
        //}

    }

    refreshView();
}

void VolumeViewer::setOverlayName(QString name){

    if( name==m_overlayName) return;

    m_overlayName=name;

    m_overlayColorBarDock->setWindowTitle(QString("%1 - Colortable").arg(name) );
}

void VolumeViewer::setOverlayPercentage(int p){

    if( p==m_overlayPercentage ) return;

    m_overlayPercentage=p;

    emit overlayPercentageChanged(p);

    refreshView();
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

    Q_ASSERT( m_volume);

    ui->openGLWidget->makeCurrent(); // need this to access currect VBOs

    RenderScene* scene=view->scene();
    //if( !scene ) return;
    Q_ASSERT(scene);

    scene->clear();

    Grid3DBounds bounds=m_volume->bounds();
    QVector<int> ilines;
    QVector<int> xlines;
    QVector<int> msecs;
    // volume outline
    if( ui->actionShow_Volume_Outline->isChecked()){
        outlineToView( bounds, m_outlineColor);

        // add min/max inline and crossline and time to labels if outline is drawn
        ilines<<bounds.inline1();
        ilines<<bounds.inline2();
        xlines<<bounds.crossline1();
        xlines<<bounds.crossline2();
        msecs<<static_cast<int>(1000*bounds.ft());
        msecs<<static_cast<int>(1000*bounds.lt());
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


    // labels
    if( ui->actionShow_Labels->isChecked()){

        //int topMS=1000*bounds.ft();
        //int bottomMS=1000*bounds.lt();

        // inlines
        foreach( int iline, ilines){

            QPointF xy1=ilxl_to_xy.map( QPoint(iline, bounds.crossline1() ) );
            QPointF xy2=ilxl_to_xy.map( QPoint(iline, bounds.crossline2() ) );
            QString str=QString("iline %1").arg(iline);
            textToView( QVector3D( xy1.x(), bounds.ft(), xy1.y()), QVector3D( xy2.x(), bounds.ft(), xy2.y()),
                        str, Qt::AlignHCenter|Qt::AlignBottom);
            textToView( QVector3D( xy2.x(), bounds.lt(), xy2.y()), QVector3D( xy1.x(), bounds.lt(), xy1.y()),
                        str, Qt::AlignHCenter|Qt::AlignTop);
        }


        // crosslines
        foreach( int xline, xlines){

            QPointF xy1=ilxl_to_xy.map( QPoint(bounds.inline1(), xline ) );
            QPointF xy2=ilxl_to_xy.map( QPoint(bounds.inline2(), xline ) );
            QString str=QString("xline %1").arg(xline);
            textToView( QVector3D( xy1.x(), bounds.ft(), xy1.y()), QVector3D( xy2.x(), bounds.ft(), xy2.y()),
                        str, Qt::AlignHCenter|Qt::AlignBottom);
            textToView( QVector3D( xy2.x(), bounds.lt(), xy2.y()), QVector3D( xy1.x(), bounds.lt(), xy1.y()),
                        str, Qt::AlignHCenter|Qt::AlignTop);
        }

        // times / msecs
        QPointF xy1=ilxl_to_xy.map( QPoint(bounds.inline1(), bounds.crossline1() ) );
        QPointF xy2=ilxl_to_xy.map( QPoint(bounds.inline1(), bounds.crossline2() ) );
        QPointF xy3=ilxl_to_xy.map( QPoint(bounds.inline2(), bounds.crossline2() ) );
        QPointF xy4=ilxl_to_xy.map( QPoint(bounds.inline2(), bounds.crossline1() ) );
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
        Grid2D<float>* h = hdef.horizon.get();

        if( h ){

            if( hdef.useColor ){
                horizonToView(h, hdef.color, hdef.colorScaler, hdef.delay);
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

QColor mix(QColor color1, QColor color2, qreal r){

    return QColor::fromRgbF( (1.-r)*color1.redF() + r * color2.redF(),
                    (1.-r)*color1.greenF() + r * color2.greenF(),
                    (1.-r)*color1.blueF() + r* color2.blueF(),
                    1. - (1.-r)*(1.-color1.alphaF()) + r*(1.-color2.alphaF() ) );
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
        }
    }

    // overlay
    if( m_overlayVolume ){

        QImage oimg(bounds.crosslineCount(), bounds.sampleCount(), QImage::Format_RGBA8888);
        oimg.fill(qRgba(0,0,0,0));
        Grid3DBounds obounds=m_overlayVolume->bounds();

        for( int i=0; i<oimg.width(); i++){
            int xline = bounds.crossline1() + i;
            for( int j=0; j<oimg.height(); j++){
                float t=bounds.sampleToTime(j);
                Grid3D<float>::value_type value=m_overlayVolume->value( iline, xline, t);  // returns NULL if outside
                if( value == m_overlayVolume->NULL_VALUE ) continue;
                ColorTable::color_type color=m_overlayColorTable->map(value);
                oimg.setPixel(i,j, color );
            }
        }

        img=mix(img, oimg, m_overlayPercentage);
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

    // overlay
    if( m_overlayVolume ){

        QImage oimg(bounds.inlineCount(), bounds.sampleCount(), QImage::Format_RGBA8888);
        oimg.fill(qRgba(0,0,0,0));
        Grid3DBounds obounds=m_overlayVolume->bounds();

        for( int i=0; i<oimg.width(); i++){
            int iline = bounds.inline1() + i;
            for( int j=0; j<oimg.height(); j++){
                float t=bounds.sampleToTime(j);
                Grid3D<float>::value_type value=m_overlayVolume->value( iline, xline, t);  // returns NULL if outside
                if( value == m_overlayVolume->NULL_VALUE ) continue;
                ColorTable::color_type color=m_overlayColorTable->map(value);
                oimg.setPixel(i,j, color );
            }
        }

        img=mix(img, oimg, m_overlayPercentage);
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
        }
    }

    // overlay
    if( m_overlayVolume ){

        QImage oimg(bounds.inlineCount(), bounds.crosslineCount(), QImage::Format_RGBA8888);
        oimg.fill(qRgba(0,0,0,0));
        Grid3DBounds obounds=m_overlayVolume->bounds();

        for( int il=bounds.inline1(); il<=bounds.inline2(); il++){
            for( int xl=bounds.crossline1(); xl<=bounds.crossline2(); xl++){

                Grid3D<float>::value_type value=m_overlayVolume->value( il, xl, time); // returns NULL if outside
                if( value == m_overlayVolume->NULL_VALUE ) continue;

                int xi=il-bounds.inline1();
                int yi=xl-bounds.crossline1();

                ColorTable::color_type color=m_overlayColorTable->map(value);
                oimg.setPixel(xi,yi, color );
            }
        }

        img=mix(img, oimg, m_overlayPercentage);

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

    qreal r = qreal(m_overlayPercentage)/100;

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
                    float red=static_cast<float>(c.redF());
                    float green=static_cast<float>(c.greenF());
                    float blue=static_cast<float>(c.blueF());

                    if( m_overlayVolume ){

                        float ovalue=m_overlayVolume->value(iline, xline, t);
                        if( ovalue!=m_overlayVolume->NULL_VALUE){
                            QColor oc=m_overlayColorTable->map(ovalue);
                            c = mix( c, oc, r);
                        }

                    }

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


void VolumeViewer::horizonToView(Grid2D<float>* hrz, QColor hcolor, qreal scaler, int delayMSec){

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

                    color = baseColor * ( 1 - x * scaler);//  (2.-x)/2;
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
void VolumeViewer::textToView( QVector3D pos1, QVector3D pos2, QString text, Qt::Alignment align ){

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
    if( !m_volume ) return;

    if(!displayRangeDialog){

        displayRangeDialog=new HistogramRangeSelectionDialog(this);
        displayRangeDialog->setHistogram(createHistogram( std::begin(*m_volume), std::end(*m_volume),
                                                          m_volume->NULL_VALUE, 100 ));
        displayRangeDialog->setDefaultRange( m_volume->valueRange());
        displayRangeDialog->setColorTable(m_colorTable );   // all updating through colortable

        displayRangeDialog->setWindowTitle("Configure Volume Display Range" );
    }


    displayRangeDialog->show();

}

void VolumeViewer::on_actionOverlay_Colortable_triggered()
{
    QVector<QRgb> oldColors=m_overlayColorTable->colors();

    ColorTableDialog colorTableDialog( oldColors );

    colorTableDialog.setWindowTitle(tr("Configure Overlay Colortable"));

    if( colorTableDialog.exec()==QDialog::Accepted ){
        m_overlayColorTable->setColors( colorTableDialog.colors());
    }else{
        m_overlayColorTable->setColors( oldColors );
    }

}

void VolumeViewer::on_actionOverlay_Range_triggered()
{
    if( !m_overlayVolume ) return;

    if(!overlayRangeDialog){

        overlayRangeDialog=new HistogramRangeSelectionDialog(this);
        overlayRangeDialog->setHistogram(createHistogram( std::begin(*m_overlayVolume), std::end(*m_overlayVolume),
                                                          m_overlayVolume->NULL_VALUE, 100 ));
        overlayRangeDialog->setDefaultRange( m_overlayVolume->valueRange());
        overlayRangeDialog->setColorTable(m_overlayColorTable );   // all updating through colortable

        overlayRangeDialog->setWindowTitle("Configure Overlay Display" );

        overlayRangeDialog->setWindowTitle(tr("Configure Overlay Display Range"));
    }

    overlayRangeDialog->show();
}

void VolumeViewer::on_actionOverlay_Percentage_triggered()
{
    if( !overlayPercentageDialog ){

        overlayPercentageDialog = new OverlayPercentageDialog(this);
        overlayPercentageDialog->setWindowTitle(tr("Volume Overlay Percentage"));
        overlayPercentageDialog->setPercentage(m_overlayPercentage);
        connect( overlayPercentageDialog, SIGNAL(percentageChanged(int)), this, SLOT(setOverlayPercentage(int)) );
        connect( this, SIGNAL(overlayPercentageChanged(int)), overlayPercentageDialog, SLOT(setPercentage(int)) );
    }

    overlayPercentageDialog->show();
}

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
    viewer->setWindowTitle(QString("Histogram of %1").arg(m_volumeName ) );
    viewer->setColorTable(m_colorTable);        // the colortable must have same parent as viewer, maybe used shared_ptr!!!
    viewer->show();
}

void VolumeViewer::on_actionOverlay_Histogram_triggered()
{
    if( ! m_overlayVolume ) return;

    QVector<double> data;
    for( auto it=m_overlayVolume->values().cbegin(); it!=m_overlayVolume->values().cend(); ++it){
        if( *it==m_overlayVolume->NULL_VALUE) continue;
        data.push_back(*it);
     }

    HistogramDialog* viewer=new HistogramDialog(this);      // need to make this a parent in order to allow qt to delete this when this is deleted
                                                            // this is important because otherwise the colortable will be deleted before this! CRASH!!!
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1").arg(m_overlayName) );
    viewer->setColorTable(m_overlayColorTable);        // the colortable must have same parent as viewer, maybe used shared_ptr!!!
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
    setVolumeName(name);
}

void VolumeViewer::on_actionOpen_Overlay_Volume_triggered()
{
    if( !m_project )return;

    bool ok=false;
    QString name=QInputDialog::getItem( this, tr("Open Overlay Volume"), tr("Select Volume"),
                                        m_project->volumeList(), 0, false, &ok);

    if( !ok || name.isNull() ) return;

    std::shared_ptr<Grid3D<float> > volume=m_project->loadVolume( name);
    if( !volume ){
        QMessageBox::critical(this, tr("Open Volume"), tr("Loading Volume failed!"));
        return;
    }

    setOverlayVolume(volume);
    setOverlayName(name);
}

void VolumeViewer::on_actionClose_Overlay_Volume_triggered()
{

    setOverlayVolume( std::shared_ptr<Grid3D<float>>() );
    setOverlayName("");
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


