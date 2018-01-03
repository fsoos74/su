#include "volumeviewer2d.h"
#include "ui_volumeviewer2d.h"

#include <QWidget>
#include <QToolBar>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QColorDialog>
#include <avoproject.h>
#include <qprogressbar.h>

#include <colortabledialog.h>
#include <multiinputdialog.h>
#include <multiitemselectiondialog.h>
#include <histogramcreator.h>
#include <volumeitemsconfigdialog.h>
#include <playerdialog.h>

#include <topsdbmanager.h>


const char* NO_HORIZON="NONE";

VolumeViewer2D::VolumeViewer2D(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VolumeViewer2D)
{
    ui->setupUi(this);

    m_colorbarsLayout=new QVBoxLayout(ui->wdColortableArea);

    setupToolBarControls();

    setAttribute( Qt::WA_DeleteOnClose);

    ui->volumeView->setZoomMode(AxisView::ZOOMMODE::BOTH);
    ui->volumeView->setCursorMode(AxisView::CURSORMODE::BOTH);
    ui->volumeView->hscaleView()->setSelectionMode(AxisView::SELECTIONMODE::SinglePoint);
    ui->volumeView->vscaleView()->setSelectionMode(AxisView::SELECTIONMODE::SinglePoint);
    ui->volumeView->setHScaleAlignment(Qt::AlignTop);
    ui->volumeView->setVScaleAlignment(Qt::AlignLeft);

    connect( ui->volumeView, SIGNAL(volumesChanged()), this, SLOT(onVolumesChanged()) );
    connect( ui->volumeView, SIGNAL(sliceChanged(VolumeView::SliceDef)), this, SLOT(onSliceChanged(VolumeView::SliceDef)));
    connect( ui->volumeView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)));

    connect( ui->actionDisplay_Horizons, SIGNAL(toggled(bool)), ui->volumeView, SLOT(setDisplayHorizons(bool)) );
    connect( ui->volumeView, SIGNAL(displayHorizonsChanged(bool)), ui->actionDisplay_Horizons, SLOT(setChecked(bool)) );

    connect( ui->actionDisplay_Wells, SIGNAL(toggled(bool)), ui->volumeView, SLOT(setDisplayWells(bool)) );
    connect( ui->volumeView, SIGNAL(displayWellsChanged(bool)), ui->actionDisplay_Wells, SLOT(setChecked(bool)) );

    connect( ui->actionDisplay_Markers, SIGNAL(toggled(bool)), ui->volumeView, SLOT(setDisplayMarkers(bool)) );
    connect( ui->volumeView, SIGNAL(displayMarkersChanged(bool)), ui->actionDisplay_Markers, SLOT(setChecked(bool)) );

    connect( ui->actionDisplay_Marker_Labels, SIGNAL(toggled(bool)), ui->volumeView, SLOT(setDisplayMarkerLabels(bool)) );
    connect( ui->volumeView, SIGNAL(displayMarkerLabelsChanged(bool)), ui->actionDisplay_Marker_Labels, SLOT(setChecked(bool)) );

    connect( ui->actionDisplay_Last_Viewed, SIGNAL(toggled(bool)), ui->volumeView, SLOT(setDisplayLastViewed(bool)) );
    connect( ui->volumeView, SIGNAL(displayLastViewedChanged(bool)), ui->actionDisplay_Last_Viewed, SLOT(setChecked(bool)) );

    connect( ui->actionBack, SIGNAL(triggered(bool)), ui->volumeView, SLOT(back()));

    updateSliceConnections();

    ui->volumeView->setSlice(VolumeView::SliceType::Inline, 0);
}

VolumeViewer2D::~VolumeViewer2D()
{
    delete ui;
}

void VolumeViewer2D::setProject(AVOProject * p){
    m_project=p;
    if( !p) return;

    QTransform xy_to_ilxl, ilxl_to_xy;
    p->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);
    ui->volumeView->setTransforms(xy_to_ilxl, ilxl_to_xy);

    updateFlattenHorizons();
}

void VolumeViewer2D::addVolume(QString name){

    if( !m_project ) return;

    auto volume=m_project->loadVolume(name, true);
    if( !volume) return;

    ui->volumeView->addVolume(name, volume);

    if( ui->volumeView->volumeList().size()==1){
        ui->volumeView->setSlice(VolumeView::SliceType::Inline, volume->bounds().i1());
    }
 }

void VolumeViewer2D::cbSlice_currentIndexChanged(QString s)
{
    if(m_noupdate) return;

    auto t=VolumeView::toSliceType(s);
    auto v=sbSlice->value();
    v=ui->volumeView->adjustToVolume(t,v);
    ui->volumeView->setSlice(t,v);
}

void VolumeViewer2D::sbSlice_valueChanged(int arg1)
{
    if(m_noupdate) return;

    auto t=ui->volumeView->slice().type;
    ui->volumeView->setSlice(t, arg1);
}

void VolumeViewer2D::setInlineSliceX(QPointF p){

    int il=static_cast<int>(std::round(p.x()));
    ui->volumeView->setSlice(VolumeView::SliceType::Inline, il);
}

void VolumeViewer2D::setInlineSliceY(QPointF p){

    int il=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceType::Inline, il);
}

void VolumeViewer2D::setCrosslineSliceX(QPointF p){

    int xl=static_cast<int>(std::round(p.x()));
    ui->volumeView->setSlice(VolumeView::SliceType::Crossline, xl);
}

void VolumeViewer2D::setCrosslineSliceY(QPointF p){

    int xl=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceType::Crossline, xl);
}

void VolumeViewer2D::setZSlice(QPointF p){

    int z=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceType::Z, z);
}

void VolumeViewer2D::setFlattenHorizon(QString name){

    if( !m_project) return;

    std::shared_ptr<Grid2D<float>> hrz;
    if( !name.isEmpty() && name!=NO_HORIZON ){
        hrz=m_project->loadGrid(GridType::Horizon, name);
        if( !hrz ){
            QMessageBox::critical(this, tr("Load Horizon"), tr("Loading Horizon \"%1\" failed!").arg(name), QMessageBox::Ok);
        }
    }
    ui->volumeView->setFlattenHorizon(hrz);
}

void VolumeViewer2D::updateSliceConnections(){

    VolumeView::SliceType type=ui->volumeView->slice().type;

    HScaleView* hsv=ui->volumeView->hscaleView();
    VScaleView* vsv=ui->volumeView->vscaleView();
    disconnect(hsv, SIGNAL(pointSelected(QPointF)),0,0);
    disconnect(vsv, SIGNAL(pointSelected(QPointF)),0,0);

    switch(type){
    case VolumeView::SliceType::Inline:
         connect( hsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setCrosslineSliceX(QPointF)) );
         connect( vsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setZSlice(QPointF)) );
         break;

    case VolumeView::SliceType::Crossline:
         connect( hsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setInlineSliceX(QPointF)) );
         connect( vsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setZSlice(QPointF)) );
         break;

    case VolumeView::SliceType::Z:
         connect( hsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setInlineSliceX(QPointF)) );
         connect( vsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setCrosslineSliceY(QPointF)) );
         break;
    }

}


void VolumeViewer2D::setupToolBarControls(){

    auto widget=new QWidget(this);
    auto label=new QLabel(tr("Slice:"));
    cbSlice=new QComboBox;
    cbSlice->addItem( VolumeView::toQString(VolumeView::SliceType::Inline));
    cbSlice->addItem( VolumeView::toQString(VolumeView::SliceType::Crossline));
    cbSlice->addItem( VolumeView::toQString(VolumeView::SliceType::Z));
    sbSlice=new QSpinBox;
    //pbSlice=new QPushButton("Back");

    auto label2=new QLabel(tr("Well Dist:"));
    sbWellViewDist=new QSpinBox;

    auto label3=new QLabel(tr("Flatten Horizon:"));
    cbHorizon=new QComboBox;

    auto layout=new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(cbSlice);
    layout->addWidget(sbSlice);
    //layout->addWidget(pbSlice);
    layout->addSpacing(30);
    layout->addWidget(label3);
    layout->addWidget(cbHorizon);
    layout->addSpacing(20);
    layout->addWidget(label2);
    layout->addWidget(sbWellViewDist);

    widget->setLayout(layout);

    auto toolBar=addToolBar(tr("Controls Toolbar"));
    toolBar->addWidget(widget);

    connect(cbSlice, SIGNAL(currentIndexChanged(QString)), this, SLOT(cbSlice_currentIndexChanged(QString)) );
    connect(sbSlice, SIGNAL(valueChanged(int)), this, SLOT(sbSlice_valueChanged(int)) );
    //connect(pbSlice, SIGNAL(clicked(bool)), ui->volumeView, SLOT(back()));
    connect(sbWellViewDist, SIGNAL(valueChanged(int)), ui->volumeView, SLOT(setWellViewDist(int)) );
    connect(cbHorizon, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFlattenHorizon(QString)));

    sbWellViewDist->setRange(0, 9999);
    sbWellViewDist->setValue(ui->volumeView->welViewDist());
}

void VolumeViewer2D::updateFlattenHorizons(){

    cbHorizon->clear();
    cbHorizon->addItem(NO_HORIZON);

    if( !m_project) return;

    for( auto name:m_project->gridList(GridType::Horizon)){
        cbHorizon->addItem(name);
    }
}

void VolumeViewer2D::on_actionDisplay_Range_triggered()
{
    QString name;
    auto vlist=ui->volumeView->volumeList();
    if( vlist.isEmpty()){
        return;
    }
    else if( vlist.size()==1){
        name=vlist.front();
    }
    else{
        bool ok=false;
        name = QInputDialog::getItem( this, tr("Volume Display Range"), tr("Select Volume:"), vlist, 0, false, &ok);
        if( !ok || name.isEmpty()) return;
    }

    auto volume=ui->volumeView->volume(name);
    if( !volume) return;

    auto ct=ui->volumeView->volumeColorTable(name);
    if( !ct ) return;

    if( !displayRangeDialog ){

        displayRangeDialog=new HistogramRangeSelectionDialog(this);
        displayRangeDialog->setWindowTitle("Configure Display Range" );

        if( !m_volumeHistograms.contains(name)){
            //QProgressBar* pbar=new QProgressBar(this);
            //pbar->setRange(0,100);
            //statusBar()->addPermanentWidget(pbar,1);
            HistogramCreator hcreator;
            //connect( &hcreator, SIGNAL(percentDone(int)), pbar, SLOT(setValue(int)) );
            auto histogram=hcreator.createHistogram( QString("Volume %1").arg(name), std::begin(*volume), std::end(*volume), volume->NULL_VALUE, 100 );
            //delete pbar;
            m_volumeHistograms.insert(name, histogram);
        }
/*
        if( !m_volumeRanges.contains(name)){
            auto rg=volume->valueRange();
            m_volumeRanges.insert(name, rg);
        }
*/
        displayRangeDialog->setHistogram(m_volumeHistograms.value(name));
        displayRangeDialog->setRange(ct->range() );//  m_volumeRanges.value(name));
        displayRangeDialog->setColorTable( ct );   // all updating through colortab
    }

    displayRangeDialog->show();
  //  m_displayRangeDialogVolume=name;
}

void VolumeViewer2D::on_actionColorbar_triggered()
{

    QString name;
    auto vlist=ui->volumeView->volumeList();
    if( vlist.isEmpty()){
        return;
    }
    else if( vlist.size()==1){
        name=vlist.front();
    }
    else{
        bool ok=false;
        name = QInputDialog::getItem( this, tr("Volume Colortable"), tr("Select Volume:"), vlist, 0, false, &ok);
        if( !ok || name.isEmpty()) return;
    }

    auto ct=ui->volumeView->volumeColorTable(name);
    if( !ct ) return;

    QVector<QRgb> oldColors=ct->colors();

    ColorTableDialog colorTableDialog( oldColors);

    if( colorTableDialog.exec()==QDialog::Accepted ){
        ct->setColors( colorTableDialog.colors());
    }else{
        ct->setColors( oldColors );
    }
}


void VolumeViewer2D::on_actionVolume_Opacity_triggered()
{
    QString name;
    auto vlist=ui->volumeView->volumeList();
    if( vlist.isEmpty()){
        return;
    }
    else if( vlist.size()==1){
        name=vlist.front();
    }
    else{
        bool ok=false;
        name = QInputDialog::getItem( this, tr("Volume Opacity"), tr("Select Volume:"), vlist, 0, false, &ok);
        if( !ok || name.isEmpty()) return;
    }

    double o=ui->volumeView->volumeAlpha(name);
    bool ok=false;
    o=QInputDialog::getDouble(this, tr("Volume Opacity"), tr("Opacity:"), o, 0, 1, 2, &ok);
    if( !ok ) return;

    ui->volumeView->setVolumeAlpha(name, o);
}


void VolumeViewer2D::on_action_Configure_Volumes_triggered()
{
    VolumeItemsConfigDialog* dlg=new VolumeItemsConfigDialog();
    dlg->setWindowTitle(tr("Configure Volumes"));

    for( auto name : ui->volumeView->volumeList()){

        if( !m_volumeHistograms.contains(name)){
            auto volume=ui->volumeView->volume(name);
            if( !volume ) continue;
            HistogramCreator hcreator;
            auto histogram=hcreator.createHistogram( QString("Volume %1").arg(name), std::begin(*volume), std::end(*volume), volume->NULL_VALUE, 100 );
            m_volumeHistograms.insert(name, histogram);
        }
        dlg->addVolumeItem(name, ui->volumeView->volumeAlpha(name), ui->volumeView->volumeColorTable(name), m_volumeHistograms.value(name) );
    }

    connect(dlg,SIGNAL(alphaChanged(QString,double)), ui->volumeView, SLOT(setVolumeAlpha(QString, double)) );

    dlg->exec();
}



void VolumeViewer2D::onVolumesChanged(){

    auto volumes = ui->volumeView->volumeList();

    // remove colorbars of non-existent volumes
    for( auto name : m_colorbars.keys()){
        if( !volumes.contains(name)){
            auto cb=m_colorbars.value(name);
            if( cb ) delete cb;
            m_colorbars.remove(name);
        }
    }

    // add colorbars for newly added volumes
    for( auto name : volumes ){

        if( m_colorbars.contains(name) ) continue;

        auto colorbar = new ColorBarWidget(this);
        m_colorbarsLayout->addWidget(colorbar);
        auto colortable=ui->volumeView->volumeColorTable(name);
        colorbar->setColorTable(colortable);
        colorbar->setLabel(name);
        colorbar->show();
        m_colorbars.insert(name, colorbar);
    }
}


void VolumeViewer2D::onSliceChanged(VolumeView::SliceDef d){

    if( m_noupdate) return;

    m_noupdate=true;
    cbSlice->setCurrentText(VolumeView::toQString(d.type));

    int min=0;
    int max=0;
    int step=1;
    Grid3DBounds bounds=ui->volumeView->bounds();

    switch(d.type){
    case VolumeView::SliceType::Inline:{
         min=bounds.i1();
         max=bounds.i2();
         step=1;
         break;
    }
    case VolumeView::SliceType::Crossline:{
         min=bounds.j1();
         max=bounds.j2();
         step=1;
         break;
    }
    case VolumeView::SliceType::Z:{
         min=static_cast<int>(1000 * bounds.ft() ); // msecs
         max=static_cast<int>(1000 * bounds.lt() ); // msecs
         step=static_cast<int>(1000 * bounds.dt() ); // msecs
         break;
    }
    }

    sbSlice->setRange(min,max);
    sbSlice->setSingleStep(step);
    sbSlice->setValue(d.value);

    m_noupdate=false;

    updateSliceConnections();
}

void VolumeViewer2D::onMouseOver(QPointF p){

    int il,xl;
    float z;

    switch(ui->volumeView->slice().type){
    case VolumeView::SliceType::Inline:
        il=sbSlice->value();
        xl=static_cast<int>(std::round(p.x()));
        z=p.y();
        break;
    case VolumeView::SliceType::Crossline:
        xl=sbSlice->value();
        il=static_cast<int>(std::round(p.x()));
        z=p.y();
        break;
    case VolumeView::SliceType::Z:
        z=sbSlice->value();
        il=static_cast<int>(std::round(p.x()));
        xl=static_cast<int>(std::round(p.y()));
        break;
    }

    QString msg;
    msg.sprintf("iline=%d, xline=%d, time/depth=%g", il, xl, z);

    auto vlist=ui->volumeView->volumeList();
    for( auto name : vlist ){
        msg.append(QString(", %1=").arg(name));
        auto v=ui->volumeView->volume(name);
        auto value=v->value(il, xl, 0.001*z);    // z->secs
        if( value!=v->NULL_VALUE){
            msg.append(QString::number(value));
        }
        else{
            msg.append("NULL");
        }
    }

    statusBar()->showMessage(msg);
}



void VolumeViewer2D::on_actionSetup_Volumes_triggered()
{
    Q_ASSERT(m_project);

    QStringList avail=m_project->volumeList();
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Setup Volumes"), tr("Select Volumes:"), avail, &ok, ui->volumeView->volumeList());
    if( !ok ) return;

    // first pass remove items
    for( auto name : ui->volumeView->volumeList() ){

        if( !names.contains(name)) ui->volumeView->removeVolume(name);
        else names.removeAll(name);
    }

    // now we only have names that need to be added
    // second pass add items
    for( auto name : names){

        auto v = m_project->loadVolume(name, true);

        if( !v ){
            QMessageBox::critical(this, tr("Add Volume"), QString("Loading volume \"%1\" failed!").arg(name) );
            break;
        }

        ui->volumeView->addVolume(name, v);
    }
}



void VolumeViewer2D::on_actionSetup_Horizons_triggered()
{
    Q_ASSERT(m_project);

    QStringList avail=m_project->gridList(GridType::Horizon);
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Setup Horizons"), tr("Select Grids:"), avail, &ok, ui->volumeView->horizonList());
    if( !ok ) return;

    // first pass remove items
    for( auto name : ui->volumeView->horizonList() ){

        if( !names.contains(name)) ui->volumeView->removeHorizon(name);
        else names.removeAll(name);
    }

    // now we only have names that need to be added
    // second pass add items
    for( auto name : names){

        auto g = m_project->loadGrid(GridType::Horizon, name);

        if( !g ){
            QMessageBox::critical(this, tr("Add Grid"), QString("Loading grid \"%1\" failed!").arg(name) );
            break;
        }

        ui->volumeView->addHorizon(name, g, Qt::red);
    }
}

void VolumeViewer2D::on_actionSet_Horizon_Color_triggered()
{
    bool ok=false;
    QString name=QInputDialog::getItem(this, tr("Set Horizon Color"), tr("Select Horizon:"), ui->volumeView->horizonList(), 0, false, &ok);

    if(!ok || name.isEmpty()) return;

    QColor initial=ui->volumeView->horizonColor(name);
    QColor color=QColorDialog::getColor( initial, this, QString("Horizon %1 color").arg(name));

    if( color.isValid()){
        ui->volumeView->setHorizonColor(name, color);
    }
}

void VolumeViewer2D::on_actionSetup_Wells_triggered()
{
    Q_ASSERT(m_project);
try{
    QStringList avail=m_project->wellList();
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Setup Wells"), tr("Select Wells:"), avail, &ok, ui->volumeView->wellList());
    if( !ok ) return;

    // first pass remove items
    for( auto name : ui->volumeView->wellList() ){

        if( !names.contains(name)) ui->volumeView->removeWell(name);
        else names.removeAll(name);
    }

    // now we only have names that need to be added
    // second pass add items
    for( auto name : names){

        auto pt = m_project->loadWellPath(name);

        if( !pt ){
            QMessageBox::critical(this, tr("Add Well"), QString("Loading well path \"%1\" failed!").arg(name) );
            break;
        }

        auto db=m_project->openTopsDatabase();
        auto tmp=db->markersByWell(name);
        auto wms=std::make_shared<WellMarkers>(WellMarkers(tmp.begin(), tmp.end()));

        ui->volumeView->addWell(name, pt, wms);
    }
}catch(std::exception& ex){
        QMessageBox::critical(this, "Exception", ex.what(), QMessageBox::Ok);
    }
}

void VolumeViewer2D::on_actionSet_Well_Color_triggered()
{
    QColor color=QColorDialog::getColor( ui->volumeView->wellColor(), this, "Well Color");

    if( color.isValid()){
        ui->volumeView->setWellColor(color);
    }
}

void VolumeViewer2D::on_actionSet_Marker_Color_triggered()
{
    QColor color=QColorDialog::getColor( ui->volumeView->markerColor(), this, "Marker Color");

    if( color.isValid()){
        ui->volumeView->setMarkerColor(color);
    }
}


void VolumeViewer2D::on_actionSet_Last_Viewed_Color_triggered()
{
    QColor color=QColorDialog::getColor( ui->volumeView->wellColor(), this, "Last Viewed Color");

    if( color.isValid()){
        ui->volumeView->setLastViewedColor(color);
    }
}



void VolumeViewer2D::on_action_Player_triggered()
{
    if( !m_player){
        m_player=new PlayerDialog(this);
        m_player->setWindowTitle("Player");
        m_player->setBounds(ui->volumeView->bounds());
        m_player->setType( ui->volumeView->slice().type);
        m_player->setCurrent(ui->volumeView->slice().value);
        connect( m_player, SIGNAL( sliceRequested(VolumeView::SliceDef)), ui->volumeView, SLOT(setSlice(VolumeView::SliceDef)) );
    }
    m_player->show();
}


void VolumeViewer2D::on_actionSetup_Tops_triggered()
{
    QStringList avail=ui->volumeView->markersList();
    std::sort( avail.begin(), avail.end());

    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Setup Tops"), tr("Select Tops:"), avail, &ok, ui->volumeView->displayedMarkers());
    if( !ok ) return;

    ui->volumeView->setDisplayedMarkers(names);
}

/*
void VolumeViewer2D::orientate(){

    if( !m_project) return;

    auto geom=m_project->geometry();

    if( !isValid(geom)) return false;

    // p0: origin, p1: far end 1st inline, p2: far end first xline

    qreal dx10=(geom.coords(1).x()-geom.coords(0).x());
    qreal dy10=(geom.coords(1).y()-geom.coords(0).y());

    qreal dx20=(geom.coords(2).x()-geom.coords(0).x());
    qreal dy20=(geom.coords(2).y()-geom.coords(0).y());

    AxxisOrientation ilOrientation=(std::fabs(dx10) > std::fabs(dy10))?
                AxxisOrientation::Horizontal : AxxisOrientation::Vertical;

    AxxisDirection ilDirection;
    AxxisDirection xlDirection;
    if( ilOrientation==AxxisOrientation::Horizontal){

        ilDirection=(dy20<0)?AxxisDirection::Ascending : AxxisDirection::Descending; // count from equator
        xlDirection=(dx10>0)?AxxisDirection::Ascending : AxxisDirection::Descending;
    }else{

        ilDirection=(dx20>0)?AxxisDirection::Ascending : AxxisDirection::Descending;
        xlDirection=(dy10<0)?AxxisDirection::Ascending : AxxisDirection::Descending; // count from equator
    }

    gridView()->setInlineOrientation(ilOrientation);
    gridView()->setInlineDirection(ilDirection);
    gridView()->setCrosslineDirection(xlDirection);

    return true;
}
*/
