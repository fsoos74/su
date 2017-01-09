#include "volumeviewer.h"
#include "ui_volumeviewer.h"
#include <colortable.h>
#include <colortabledialog.h>
#include <modaldisplayrangedialog.h>
#include<QInputDialog>
#include<QMessageBox>
#include<QColorDialog>
#include<addslicedialog.h>

#include<QToolBar>
#include<volumenavigationwidget.h>

#include <cmath>


VolumeViewer::VolumeViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::VolumeViewer),
    m_colorTable(new ColorTable(this))
{
    ui->setupUi(this);

    m_colorTable->setColors(ColorTable::defaultColors());

    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(refreshView()) );
    connect( ui->actionShow_Volume_Outline, SIGNAL(toggled(bool)), this, SLOT(refreshView()) );
    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    setupNavigationToolBar();
    populateWindowMenu();
}

VolumeViewer::~VolumeViewer()
{
    delete ui;
}

void VolumeViewer::setupNavigationToolBar(){
    QToolBar* navigationToolBar=ui->toolBar_Navigation;
    VolumeNavigationWidget* navigation=new VolumeNavigationWidget(this);
    navigationToolBar->addWidget(navigation);
    connect( navigation, SIGNAL(positionChanged(QVector3D)), ui->openGLWidget, SLOT(setPosition(QVector3D)) );
    connect(ui->openGLWidget, SIGNAL(positionChanged(QVector3D)), navigation, SLOT(setPosition(QVector3D)) );
    connect( navigation, SIGNAL(rotationChanged(QVector3D)), ui->openGLWidget, SLOT(setRotation(QVector3D)) );
    connect(ui->openGLWidget, SIGNAL(rotationChanged(QVector3D)), navigation, SLOT(setRotation(QVector3D)) );
    connect( navigation, SIGNAL(scaleChanged(QVector3D)), ui->openGLWidget, SLOT(setScale(QVector3D)) );
    connect(ui->openGLWidget, SIGNAL(scaleChanged(QVector3D)), navigation, SLOT(setScale(QVector3D)) );
    //ui->toolBar_Navigation->hide();
}

void VolumeViewer::populateWindowMenu(){

    ui->menu_Window->addAction( ui->toolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->toolBar_View->toggleViewAction());
    ui->menu_Window->addAction( ui->toolBar_Navigation->toggleViewAction());
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

    m_volume=volume;

    if( m_volume ){

        m_colorTable->setRange(m_volume->valueRange());

        initialVolumeDisplay();

    }
}

void VolumeViewer::setHighlightedPoints(QVector<SelectionPoint> rpoints){

    m_highlightedPoints=rpoints;

    refreshView();
}


void VolumeViewer::clear(){

    m_slices.clear();
    m_horizons.clear();
    m_highlightedPoints.clear();

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


void VolumeViewer::addSlice(SliceDef def){

    if( m_slices.contains(def) ) return;            // no duplicates

    // check if valid
    Grid3DBounds bounds= m_volume->bounds();
    switch(def.type){
    case SliceType::INLINE: if( !bounds.isInside(def.value, bounds.crossline1())) return;
                            break;

    case SliceType::CROSSLINE: if( !bounds.isInside( bounds.inline1(), def.value)) return;
                            break;

    case SliceType::SAMPLE: if( !bounds.isInside( bounds.inline1(), bounds.crossline1(), def.value) ) return;
                            break;
    default:
        qFatal("Invalid SliceType");
    }

    m_slices.append(def);
    refreshView();
}

void VolumeViewer::removeSlice( SliceDef def ){

    m_slices.removeAll(def);
    refreshView();
}

void VolumeViewer::addHorizon(VolumeViewer::HorizonDef def){

    if( m_horizons.contains(def.name)) return;      // no duplicates

    if( !def.horizon ) return;                      // no null horizons

    m_horizons.insert(def.name, def );

    refreshView();
}

void VolumeViewer::removeHorizon(QString name){

    m_horizons.remove(name);

    refreshView();

    std::cout<<"Removed horizon "<<name.toStdString()<<std::endl;
}

void VolumeViewer::refreshView(){

    ViewWidget* view=ui->openGLWidget;
    Q_ASSERT(view);

    Q_ASSERT( m_volume);

    RenderScene* scene=view->scene();
    Q_ASSERT(scene);

    scene->clear();


    //directionIndicatorPlanesToView( m_volume->bounds());

    if( ui->actionShow_Volume_Outline->isChecked()){
        outlineToView( m_volume->bounds());
    }

    foreach( SliceDef slice, m_slices){
        sliceToView(slice);
    }

    foreach( HorizonDef hdef, m_horizons.values() ){
        if( hdef.horizon ){
            horizonToView(hdef.horizon.get(), hdef.color);
        }
    }

    pointsToView( m_highlightedPoints, m_highlightedPointColor, m_highlightedPointSize );

    ui->openGLWidget->update();
}

void VolumeViewer::receivePoint( SelectionPoint point, int code ){

    Q_ASSERT(m_volume);

    switch( code ){

    case PointCode::VIEWER_POINT_SELECTED:{

        if( point.iline!=SelectionPoint::NO_LINE){
            addSlice( SliceDef{SliceType::INLINE, point.iline} );
        }
        if( point.xline!=SelectionPoint::NO_LINE){
            addSlice( SliceDef{SliceType::CROSSLINE, point.xline} );
        }
        refreshView();
        break;
    }

    case PointCode::VIEWER_TIME_SELECTED:{
        int sample=point.time/m_volume->bounds().dt();              // convert from seconds
        addSlice( SliceDef{SliceType::SAMPLE, sample} );
        refreshView();
        break;
    }
    default:{               // nop

        break;
    }

    }
}

void VolumeViewer::receivePoints( QVector<SelectionPoint> points, int code){

    if( code==CODE_SINGLE_POINTS){
/*
        QVector<SelectionPoint> rpoints;
        rpoints.reserve(points.size());
        for( SelectionPoint p : points){
            rpoints.push_back(QPoint(p.iline, p.xline));
        }
*/
        setHighlightedPoints(points);
    }

}

void VolumeViewer::outlineToView(Grid3DBounds bounds){

    QVector3D color{ 1., 1., 1. };

    QPoint ILXL1(bounds.inline1(), bounds.crossline1());
    QPoint ILXL2(bounds.inline1(), bounds.crossline2());
    QPoint ILXL3(bounds.inline2(), bounds.crossline1());
    QPoint ILXL4(bounds.inline2(), bounds.crossline2());

    QPointF XY1=ilxl_to_xy.map(ILXL1);
    QPointF XY2=ilxl_to_xy.map(ILXL2);
    QPointF XY3=ilxl_to_xy.map(ILXL3);
    QPointF XY4=ilxl_to_xy.map(ILXL4);

    QVector<VIC::Vertex> vertices{
        {QVector3D( XY1.x(), bounds.ft(), XY1.y()), color },
        {QVector3D( XY2.x(), bounds.ft(), XY2.y()), color },
        {QVector3D( XY4.x(), bounds.ft(), XY4.y()), color },
        {QVector3D( XY3.x(), bounds.ft(), XY3.y()), color },
        {QVector3D( XY1.x(), bounds.lt(), XY1.y()), color },
        {QVector3D( XY2.x(), bounds.lt(), XY2.y()), color },
        {QVector3D( XY4.x(), bounds.lt(), XY4.y()), color },
        {QVector3D( XY3.x(), bounds.lt(), XY3.y()), color }
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

    std::cout<<"xmin="<<xmin<<" xmax="<<xmax<<std::endl;
    std::cout<<"ymin="<<ymin<<" ymax="<<ymax<<std::endl;
    std::cout<<"zmin="<<zmin<<" zmax="<<zmax<<std::endl;


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
    case SliceType::SAMPLE: sampleSliceToView(def.value); break;
    default: qFatal("Invalid SliceType!!!");
    }
}

void VolumeViewer::inlineSliceToView( int iline ){

    Q_ASSERT( m_volume);

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds bounds=volume.bounds();

    QImage img(bounds.crosslineCount(), bounds.sampleCount(), QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){
            img.setPixel(i,j,m_colorTable->map(volume(iline, bounds.crossline1()+i, j) ) );
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

    QImage img(bounds.inlineCount(), bounds.sampleCount(), QImage::Format_RGB32 );
    for( int i=0; i<img.width(); i++){
        for( int j=0; j<img.height(); j++){
            img.setPixel(i,j,m_colorTable->map(volume(bounds.inline1()+i, xline, j) ) );
        }
    }

    QVector<VIT::Vertex> vertices{
        {QVector3D( xline, 0,  bounds.inline1()), QVector2D( 0, 0)},   // top left
        {QVector3D( xline, 0, bounds.inline2()), QVector2D( 1, 0)},   // top right
        {QVector3D( xline, bounds.sampleCount(), bounds.inline1()), QVector2D( 0, 1)},   // bottom left
        {QVector3D( xline, bounds.sampleCount(), bounds.inline2()), QVector2D( 1, 1)}    // bottom right
    };

    QVector<VIT::Index> indices{
         0,  1,  2,  3
    };

    ui->openGLWidget->scene()->addItem( VIT::makeVIT(vertices, indices, GL_TRIANGLE_STRIP, img) );
}

void VolumeViewer::sampleSliceToView( int sample ){

    Q_ASSERT( m_volume);

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds bounds=volume.bounds();

    QImage img(bounds.inlineCount(), bounds.crosslineCount(), QImage::Format_RGB32 );
    for( int il=bounds.inline1(); il<=bounds.inline2(); il++){
        for( int xl=bounds.crossline1(); xl<=bounds.crossline2(); xl++){
            int xi=il-bounds.inline1();
            int yi=xl-bounds.crossline1();
            img.setPixel( xi, yi, m_colorTable->map(volume(il, xl, sample) ) );
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

    qreal time=bounds.sampleToTime(sample);

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

void VolumeViewer::horizonToView(Grid2D<float>* hrz, QColor hcolor){

    Q_ASSERT( m_volume );
    Q_ASSERT( hrz );

    Grid3D<float>& volume=*m_volume;
    Grid3DBounds vbounds=volume.bounds();

    Grid2DBounds hbounds=hrz->bounds();


    auto range=valueRange(*hrz);
    QVector3D nullColor{0,0,0};
    QVector3D baseColor{static_cast<float>(hcolor.redF()), static_cast<float>(hcolor.greenF()), static_cast<float>(hcolor.blueF()) };
    QVector<VIC::Vertex> vertices;

    for( int i=hbounds.i1(); i<=hbounds.i2(); i++){

        for( int j=hbounds.j1(); j<=hbounds.j2(); j++){

            float t=(*hrz)(i,j);  // horizon is in millis
            if(t!=hrz->NULL_VALUE){
                int s=vbounds.timeToSample(0.001*t);
                float x=(t-range.first)/(range.second-range.first);
                QVector3D color=baseColor*(2.-x)/2;                     // scale color dep on time/depth, deeper->darker
                vertices.append( VIC::Vertex{ QVector3D( j, s, i), color } );
            }
            else{
                vertices.append( VIC::Vertex{ QVector3D( j, 0, i), nullColor } );
            }
            //std::cout<<j<<", "<<s<<", "<<i<<std::endl;
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
/*
    foreach (int idx, indices) {
        std::cout<<idx<<std::endl;
    }
*/
    ui->openGLWidget->scene()->addItem( VIC::makeVIC(vertices, indices, GL_TRIANGLE_STRIP) );
}


void VolumeViewer::pointsToView(QVector<SelectionPoint> points, QColor color, qreal SIZE){

    Q_ASSERT( m_volume );
    Grid3DBounds bounds=m_volume->bounds();

    QVector3D drawColor{static_cast<float>(color.redF()), static_cast<float>(color.greenF()), static_cast<float>(color.blueF()) };
    QVector<VIC::Vertex> vertices;
    QVector<VIC::Index> indices;

    QVector3D scale=ui->openGLWidget->scale();
    qreal sizeX=SIZE/std::fabs(scale.x());
    qreal sizeY=SIZE/std::fabs(scale.y());
    qreal sizeZ=SIZE/std::fabs(scale.z());

    for( int i=0; i<points.size(); i++){

        const SelectionPoint& point=points[i];

        QVector3D p( point.xline, bounds.timeToSample(point.time), point.iline );

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


void VolumeViewer::initialVolumeDisplay(){

    Q_ASSERT( m_volume );

    m_slices.clear();

    Grid3DBounds bounds=m_volume->bounds();



    int iline=(bounds.inline1()+bounds.inline2())/2;
    m_slices.append( SliceDef{SliceType::INLINE, iline});

    int xline=(bounds.crossline1()+bounds.crossline2())/2;
    m_slices.append( SliceDef{SliceType::CROSSLINE, xline});

    int sample=(bounds.sampleCount())/2;
    m_slices.append( SliceDef{SliceType::SAMPLE, sample});

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
}

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

void VolumeViewer::on_actionAdd_Horizon_triggered()
{

    static const QVector<QColor> HorizonColors{Qt::darkRed, Qt::darkGreen, Qt::darkBlue,
                Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow,
                Qt::red, Qt::green, Qt::blue,
                Qt::cyan, Qt::magenta, Qt::yellow};


    if( !m_project ) return;

    bool ok=false;
    QString gridName=QInputDialog::getItem(this, "Open Horizon", "Please select a horizon:",
                                           m_project->gridList(GridType::Horizon), 0, false, &ok);
    if( !gridName.isEmpty() && ok ){

        if( m_horizons.contains(gridName)){
            QMessageBox::information(this, tr("Add Horizon"), QString("Horizon \"%1\" is already loaded!").arg(gridName));
            return;
        }

        std::shared_ptr<Grid2D<float> > grid=m_project->loadGrid(GridType::Horizon, gridName );
        if( !grid ){
            QMessageBox::critical(this, tr("Add Horizon"), QString("Loading horizon \"%1\" failed!"). arg(gridName) );
            return;
        }

        HorizonDef def{gridName, grid, HorizonColors.at(m_horizons.size()%HorizonColors.size()) };

        addHorizon(def);
    }
}

void VolumeViewer::on_actionRemove_Horizon_triggered()
{
    bool ok=false;

    QString horizon=QInputDialog::getItem(this, tr("Remove Horizon"), tr("Select Horizon"), m_horizons.keys(), 0, false, &ok);
    if( horizon.isEmpty() || !ok ) return;

    removeHorizon( horizon );
}

void VolumeViewer::on_actionChange_Horizon_Color_triggered()
{
    bool ok=false;

    QString horizon=QInputDialog::getItem(this, tr("Remove Horizon"), tr("Select Horizon"), m_horizons.keys(), 0, false, &ok);
    if( horizon.isEmpty() || !ok ) return;

    HorizonDef def=m_horizons.value(horizon);

    QColor color=QColorDialog::getColor(def.color, this, QString("Select Horizon Color \"%1\"").arg(horizon));
    if( color.isValid()){
        def.color=color;
        m_horizons[horizon].color=color;
        refreshView();
    }
}


void VolumeViewer::defaultPositionAndScale(){

    Q_ASSERT(m_volume);

    Grid3DBounds bounds=m_volume->bounds();

    QRect br_ilxl( bounds.inline1(), bounds.crossline1(), bounds.inlineCount(), bounds.crosslineCount() );
    QRectF br_xy=ilxl_to_xy.mapRect(br_ilxl);

    // compute scale factor to make lines and times same size
    qreal yfac=qreal(std::max(br_xy.width(), br_xy.height()))/(bounds.lt()-bounds.ft());

    qreal cx=br_xy.center().x();
    qreal cz=br_xy.center().y();
    qreal cy=0.5*(bounds.ft()+bounds.lt());

    qreal w=br_xy.width();
    qreal h=yfac*(bounds.lt()-bounds.ft());

    /* This is for positive z points towards user, standard
    ui->openGLWidget->setScale(QVector3D(1., -yfac, 1. ) );     // - make y-axis top to bottom!!!
    ui->openGLWidget->setCenter(QVector3D(cx,cy,cz));
    ui->openGLWidget->setPosition(QVector3D( -cx, - cy,  ( -br_xy.top() - 2*std::sqrt(w*w+h*h) ) ) );
    */


    ui->openGLWidget->setScale(QVector3D(1., -yfac, -1. ) );     // - make y-axis top to bottom, z-axis points backwards
    ui->openGLWidget->setCenter(QVector3D(cx,cy,cz));

    ui->openGLWidget->setPosition(QVector3D( -cx, - cy,  ( -br_xy.top() + 2*std::sqrt(w*w+h*h) ) ) );

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

    int s=QInputDialog::getInt( this, tr("Set Point Size"), tr("Point Size:"), m_highlightedPointSize, 1, 99, 1, &ok );
    if( !ok ) return;

    setHighlightedPointSize(s);
}
