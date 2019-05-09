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
#include <playerdialog.h>
#include <horizonitem.h>
#include <volumeitem.h>
#include <viewitemmodel.h>
#include <volumeitemsdialog.h>
#include <horizonitemsdialog.h>
#include <topsdbmanager.h>


const char* NO_HORIZON="NONE";

VolumeViewer2D::VolumeViewer2D(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VolumeViewer2D)
{
    ui->setupUi(this);

    //m_colorbarsLayout=new QVBoxLayout(ui->wdColortableArea);
    ui->splitter->setSizes(QList<int>{400,100});

    //setupToolBarControls();
    setupMouseModes();
    setupSliceToolBar();
    setupFlattenToolBar();
    setupWellToolBar();
    setupPickingToolBar();
    setupEnhanceToolBar();
    populateWindowMenu();
    m_flattenToolBar->setVisible(false);
    m_wellToolBar->setVisible(false);
    m_pickToolBar->setVisible(false);
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

    connect( ui->actionDisplay_Tables, SIGNAL(toggled(bool)), ui->volumeView, SLOT(setDisplayTables(bool)) );
    connect( ui->volumeView, SIGNAL(displayTablesChanged(bool)), ui->actionDisplay_Tables, SLOT(setChecked(bool)) );

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

    VolumeItem* vitem=new VolumeItem(this);
    vitem->setName(name);
    auto r=volume->valueRange();
    vitem->colorTable()->setRange(r);
    vitem->setVolume(volume);

    ui->volumeView->volumeItemModel()->add(vitem);

    if( ui->volumeView->volumeItemModel()->size()==1){
        ui->volumeView->setSlice(VolumeView::SliceType::Inline, volume->bounds().i1());
    }
 }

void VolumeViewer2D::cbSlice_currentIndexChanged(QString s)
{
    if(m_noupdate) return;

    auto t=VolumeView::toSliceType(s);
    auto v=m_sbSlice->value();
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

    auto vv=ui->volumeView;
    HScaleView* hsv=ui->volumeView->hscaleView();
    VScaleView* vsv=ui->volumeView->vscaleView();
    //disconnect(vv, SIGNAL(polygonSelected(QPolygonF)), 0, 0); XXX why?
    disconnect(hsv, SIGNAL(pointSelected(QPointF)),0,0);
    disconnect(vsv, SIGNAL(pointSelected(QPointF)),0,0);

    VolumeView::SliceType type=vv->slice().type;
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

void VolumeViewer2D::setupMouseModes(){

    DynamicMouseModeSelector* mm=new DynamicMouseModeSelector(this);
    mm->addMode(MouseMode::Explore);
    mm->addMode(MouseMode::Zoom);
    //mm->addMode(MouseMode::Select);
    mm->addMode(MouseMode::Pick);
    mm->addMode(MouseMode::DeletePick);
    ui->mouseToolBar->addWidget( mm);
    m_mousemodeSelector=mm;
    connect(m_mousemodeSelector, SIGNAL(modeChanged(MouseMode)), ui->volumeView, SLOT(setMouseMode(MouseMode)) );
}


void VolumeViewer2D::setupSliceToolBar(){
    m_sliceToolBar=new QToolBar( "Slice", this);
    auto widget=new QWidget(this);
    auto label=new QLabel(tr("Slice:"));
    m_cbSlice=new QComboBox;
    m_cbSlice->addItem( VolumeView::toQString(VolumeView::SliceType::Inline));
    m_cbSlice->addItem( VolumeView::toQString(VolumeView::SliceType::Crossline));
    m_cbSlice->addItem( VolumeView::toQString(VolumeView::SliceType::Z));
    m_cbSlice->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_sbSlice=new ReverseSpinBox;
    m_sbSlice->setReverse(false);
    auto layout=new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(m_cbSlice);
    layout->addWidget(m_sbSlice);
    widget->setLayout(layout);
    m_sliceToolBar->addWidget(widget);
    addToolBar(m_sliceToolBar);
    connect(m_cbSlice, SIGNAL(currentIndexChanged(QString)), this, SLOT(cbSlice_currentIndexChanged(QString)) );
    connect(m_sbSlice, SIGNAL(valueChanged(int)), this, SLOT(sbSlice_valueChanged(int)) );
}

void VolumeViewer2D::setupFlattenToolBar(){
    m_flattenToolBar=new QToolBar( "Flatten", this);
    auto widget=new QWidget(this);
    auto label=new QLabel(tr("Flatten:"));
    m_cbHorizon=new QComboBox;
    auto layout=new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(m_cbHorizon);
    widget->setLayout(layout);
    m_flattenToolBar->addWidget(widget);
    addToolBar(m_flattenToolBar);
    connect(m_cbHorizon, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFlattenHorizon(QString)));
}

void VolumeViewer2D::setupWellToolBar(){
    m_wellToolBar=new QToolBar( "Well", this);
    auto widget=new QWidget(this);
    auto label=new QLabel(tr("Dist:"));
    auto sbWellViewDist=new QSpinBox;
    auto layout=new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(sbWellViewDist);
    widget->setLayout(layout);
    m_wellToolBar->addWidget(widget);
    addToolBar(m_wellToolBar);
    connect(sbWellViewDist, SIGNAL(valueChanged(int)), ui->volumeView, SLOT(setWellViewDist(int)) );

    sbWellViewDist->setRange(0, 9999);
    sbWellViewDist->setValue(ui->volumeView->welViewDist());
}

void VolumeViewer2D::setupEnhanceToolBar(){
    m_enhanceToolBar=new QToolBar( "Enhance", this);
    auto widget=new QWidget(this);
    auto label=new QLabel(tr("Sharpen:"));
    auto sbSharpenKernelSize=new QSpinBox;
    auto sbSharpenPercent=new QSpinBox;
    auto layout=new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(sbSharpenKernelSize);
    layout->addWidget(sbSharpenPercent);
    widget->setLayout(layout);
    m_enhanceToolBar->addWidget(widget);
    addToolBar(m_enhanceToolBar);
    connect(sbSharpenKernelSize, SIGNAL(valueChanged(int)), ui->volumeView, SLOT(setSharpenKernelSize(int)));
    connect(sbSharpenPercent, SIGNAL(valueChanged(int)), ui->volumeView, SLOT(setSharpenPercent(int)) );

    sbSharpenKernelSize->setRange(3, 11);
    sbSharpenKernelSize->setSingleStep(2);
    sbSharpenKernelSize->setValue(ui->volumeView->sharpenKernelSize());
    sbSharpenPercent->setRange(0, 100);
    sbSharpenPercent->setValue(ui->volumeView->sharpenPercent());
}

void VolumeViewer2D::setupPickingToolBar(){

    m_pickToolBar=new QToolBar( "Picking", this);
    auto widget=new QWidget(this);
    auto label=new QLabel(tr("Pick:"));
    auto cbPickMode=new QComboBox(this);
    cbPickMode->addItems(volumePickModeNames());
    cbPickMode->setCurrentText(toQString(VolumePicker::PickMode::Points));
    cbPickMode->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    auto layout=new QHBoxLayout;
    layout->addWidget(label);
    layout->addWidget(cbPickMode);
    widget->setLayout(layout);
    m_pickToolBar->addWidget(widget);
    addToolBar(m_pickToolBar);
    connect( cbPickMode, SIGNAL(currentIndexChanged(QString)), ui->volumeView->picker(), SLOT(setPickMode(QString)));
}


void VolumeViewer2D::updateFlattenHorizons(){

    m_cbHorizon->clear();
    m_cbHorizon->addItem(NO_HORIZON);

    if( !m_project) return;

    for( auto name:m_project->gridList(GridType::Horizon)){
        m_cbHorizon->addItem(name);
    }
}

void VolumeViewer2D::onVolumesChanged(){

    auto volumes = ui->volumeView->volumeItemModel()->names();

    // remove colorbars of non-existent volumes
    for( auto name : m_mdiColorbars.keys()){
        if( !volumes.contains(name)){
            auto cb=m_mdiColorbars.value(name);
            if( cb ) delete cb;
            m_mdiColorbars.remove(name);
        }
    }

    // add colorbars for newly added volumes
    for( auto name : volumes){

        if(m_mdiColorbars.contains(name)) continue;        // already have color for this volume

        int idx=ui->volumeView->volumeItemModel()->indexOf(name);
        Q_ASSERT(idx>=0);
        auto vitem=dynamic_cast<VolumeItem*>(ui->volumeView->volumeItemModel()->at(idx));
        if(!vitem) continue;

        auto colorbar = new ColorBarWidget(this);
        colorbar->setMinimumSize(50,150);
        //m_colorbarsLayout->addWidget(colorbar);
        auto mdiColorbar=ui->mdiArea->addSubWindow(colorbar);
        mdiColorbar->setWindowIcon( QIcon(QPixmap(1,1)) );  // no icon
        auto colortable=vitem->colorTable();
        colorbar->setColorTable(colortable);
        colorbar->setLabel(name);
        colorbar->show();
        m_mdiColorbars.insert(name, mdiColorbar);
    }
}


void VolumeViewer2D::onSliceChanged(VolumeView::SliceDef d){

    if( m_noupdate) return;

    m_noupdate=true;
    m_cbSlice->setCurrentText(VolumeView::toQString(d.type));

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

    m_sbSlice->setRange(min,max);
    m_sbSlice->setSingleStep(step);
    m_sbSlice->setValue(d.value);
    m_sbSlice->setReverse(d.type==VolumeView::SliceType::Z);

    m_noupdate=false;

    updateSliceConnections();
}

void VolumeViewer2D::onMouseOver(QPointF p){

    int il=0,xl=0;
    float z=0;

    switch(ui->volumeView->slice().type){
    case VolumeView::SliceType::Inline:
        il=m_sbSlice->value();
        xl=static_cast<int>(std::round(p.x()));
        z=p.y();
        break;
    case VolumeView::SliceType::Crossline:
        xl=m_sbSlice->value();
        il=static_cast<int>(std::round(p.x()));
        z=p.y();
        break;
    case VolumeView::SliceType::Z:
        z=m_sbSlice->value();
        il=static_cast<int>(std::round(p.x()));
        xl=static_cast<int>(std::round(p.y()));
        break;
    }

    QString msg;
    msg.sprintf("iline=%d, xline=%d, time/depth=%g", il, xl, z);

    for(int i=0; i<ui->volumeView->volumeItemModel()->size(); i++){
        auto vitem=dynamic_cast<VolumeItem*>(ui->volumeView->volumeItemModel()->at(i));
        if(!vitem) continue;
        msg.append(QString(", %1=").arg(vitem->name()));
        auto v=vitem->volume();
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

    VolumeItemsDialog* dlg=new VolumeItemsDialog(m_project, ui->volumeView->volumeItemModel());
    dlg->setWindowTitle("Setup volumes");
    dlg->exec();
}



void VolumeViewer2D::on_actionSetup_Horizons_triggered()
{
    Q_ASSERT(m_project);

    HorizonItemsDialog* dlg=new HorizonItemsDialog(m_project, ui->volumeView->horizonItemModel());
    dlg->setWindowTitle("Setup horizons");
    dlg->exec();
}

void VolumeViewer2D::on_actionSet_Table_Color_triggered()
{
    bool ok=false;
    QString name=QInputDialog::getItem(this, tr("Set Table Color"), tr("Select Table:"), ui->volumeView->tableList(), 0, false, &ok);

    if(!ok || name.isEmpty()) return;

    QColor initial=ui->volumeView->tableColor(name);
    QColor color=QColorDialog::getColor( initial, this, QString("Table %1 color").arg(name));

    if( color.isValid()){
        ui->volumeView->setTableColor(name, color);
    }
}


void VolumeViewer2D::on_actionSetup_Tables_triggered()
{
    Q_ASSERT(m_project);

    QStringList avail=m_project->tableList();
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Setup Tables"), tr("Select tables:"), avail, &ok, ui->volumeView->tableList());
    if( !ok ) return;

    // first pass remove items
    for( auto name : ui->volumeView->tableList() ){

        if( !names.contains(name)) ui->volumeView->removeTable(name);
        else names.removeAll(name);
    }

    // now we only have names that need to be added
    // second pass add items
    for( auto name : names){

        auto t = m_project->loadTable(name);

        if( !t ){
            QMessageBox::critical(this, tr("Add Table"), QString("Loading table \"%1\" failed!").arg(name) );
            break;
        }

        ui->volumeView->addTable(name, t, Qt::blue);
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

bool VolumeViewer2D::canDiscardPicks(){
    if( !ui->volumeView->picker()->isDirty()) return true;
    auto reply = QMessageBox::warning(this, tr("Discard Picks"),
                                      tr("Picks have unsaved changes. Continuing will delete them. Continue?"),
                                        QMessageBox::Yes | QMessageBox::No );
    return reply == QMessageBox::Yes;
}

void VolumeViewer2D::on_action_New_Picks_triggered()
{
    if( !canDiscardPicks()) return;
    auto picker=ui->volumeView->picker();
    picker->newPicks();

    m_picksName.clear();
}

void VolumeViewer2D::on_action_Load_Picks_triggered()
{
    if( !canDiscardPicks()) return;

    bool ok=false;
    QString name=QInputDialog::getItem(this, "Open Picks", "Please select a Table:",
                                           m_project->tableList(), 0, false, &ok);
    if( name.isEmpty() || !ok ) return;
    auto picks=m_project->loadTable(name );
    if( !picks){
        QMessageBox::critical(this, "Load Picks", "Loading Picks failed!", QMessageBox::Ok);
        return;
    }

    auto picker=ui->volumeView->picker();
    picker->setPicks(picks);

    m_picksName=name;
}

void VolumeViewer2D::on_action_Save_Picks_triggered()
{
    if( m_picksName.isEmpty() || !m_project->existsTable(m_picksName)){
        ui->actionSave_As_Picks->trigger();
        return;
    }

    auto picker=ui->volumeView->picker();
    if( m_project->saveTable(m_picksName, *(picker->picks()) ) ){
        picker->setDirty(false);
    }
    else{
        QMessageBox::critical(this, tr("Save Picks"), tr("Saving picks failed"));
    }
}

void VolumeViewer2D::on_actionSave_As_Picks_triggered()
{
    QString name=m_picksName;
    bool ok;

    // input non-existing name
    for(;;){

        name = QInputDialog::getText(this, tr("Save Picks As"), tr("Pick Table Name:"), QLineEdit::Normal, name, &ok );

        if( name.isNull() || !ok ) return; // canceled

        if( !m_project->tableList().contains(name)){
             break;
        }
    }

    auto picker=ui->volumeView->picker();
    if( m_project->addTable(name, *(picker->picks()) ) ){
        picker->setDirty(false);
        m_picksName=name;
    }
    else{
        QMessageBox::critical(this, tr("Save Picks"), tr("Saving picks failed"));
    }

}


void VolumeViewer2D::closeEvent(QCloseEvent *event)
{
    if( !canDiscardPicks()){
        event->ignore();
        return;
    }

    /*
    sendPoint( SelectionPoint::NONE, PointCode::VIEWER_CURRENT_CDP );  // notify other viewers
    // remove intersections from other viewers
   updateIntersections();
   */
}

void VolumeViewer2D::keyPressEvent(QKeyEvent * event){

    std::cout<<"GatherViewer::keyPressEvent"<<std::endl<<std::flush;

    switch(event->key()){
    case Qt::Key_F: m_sbSlice->setValue(m_sbSlice->minimum()); break;
    case Qt::Key_L: m_sbSlice->setValue(m_sbSlice->maximum()); break;
    case Qt::Key_N: m_sbSlice->stepUp(); break;
    case Qt::Key_P: m_sbSlice->stepDown(); break;
    //case Qt::Key_Plus: zoomIn(); break;
    //case Qt::Key_Minus: zoomOut(); break;
    //case Qt::Key_0: zoomFitWindow(); break;
    default: break;
    }
    event->accept();
}

void VolumeViewer2D::populateWindowMenu(){

    ui->menu_Window->addAction( ui->toolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->mouseToolBar->toggleViewAction());
    ui->menu_Window->addAction( m_sliceToolBar->toggleViewAction());
    ui->menu_Window->addAction( m_flattenToolBar->toggleViewAction());
    ui->menu_Window->addAction( m_wellToolBar->toggleViewAction());
    ui->menu_Window->addAction( m_enhanceToolBar->toggleViewAction());
    ui->menu_Window->addAction( m_pickToolBar->toggleViewAction());
}
