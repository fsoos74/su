#include "sliceviewer.h"
#include "ui_sliceviewer.h"

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
#include <histogramcreator.h>
#include <playerdialog.h>
#include <horizonitem.h>
#include <volumeitem.h>
#include <viewitemmodel.h>
#include <volumeitemsdialog.h>
#include <horizonitemsdialog.h>
#include <wellitemsdialog.h>
#include <markeritemsdialog.h>
#include <tableitemsdialog.h>
#include <displayoptionsdialog.h>
#include <topsdbmanager.h>
#include <QActionGroup>
#include <smartcolorbarwidget.h>


namespace sliceviewer {


const char* NO_HORIZON="NONE";

SliceViewer::SliceViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SliceViewer)
{
    ui->setupUi(this);

    //m_colorbarsLayout=new QVBoxLayout(ui->wdColortableArea);
    ui->splitter->setSizes(QList<int>{400,100});

    setupMouseModes();
    setupSliceToolBar();
    setupFlattenToolBar();
    setupMuteToolBar();
    setAttribute( Qt::WA_DeleteOnClose);

    ui->volumeView->setZoomMode(AxisView::ZOOMMODE::BOTH);
    ui->volumeView->setCursorMode(AxisView::CURSORMODE::BOTH);
    ui->volumeView->hscaleView()->setSelectionMode(AxisView::SELECTIONMODE::SinglePoint);
    ui->volumeView->vscaleView()->setSelectionMode(AxisView::SELECTIONMODE::SinglePoint);
    ui->volumeView->setHScaleAlignment(Qt::AlignTop);
    ui->volumeView->setVScaleAlignment(Qt::AlignLeft);

    connect( ui->volumeView->volumeItemModel(), SIGNAL(changed()), this, SLOT(onVolumesChanged()) );
    connect( ui->volumeView->volumeItemModel(), SIGNAL(itemChanged(ViewItem*)), this, SLOT(onVolumesChanged()) );
    connect( ui->volumeView, SIGNAL(sliceChanged(VolumeView::SliceDef)), this, SLOT(onSliceChanged(VolumeView::SliceDef)));
    connect( ui->volumeView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)));
    connect( ui->volumeView, SIGNAL(inlineOrientationChanged(Qt::Orientation)), this, SLOT(updateSliceConnections()));

    connect(ui->volumeView->volumeItemModel(), SIGNAL(changed()), this, SLOT(updateColorBars()) );
    connect(ui->volumeView->volumeItemModel(), SIGNAL(itemChanged(ViewItem*)), this, SLOT(updateColorBars()) );

    connect( ui->actionBack, SIGNAL(triggered(bool)), ui->volumeView, SLOT(back()));

    auto group=new QActionGroup(this);
    group->addAction(ui->actionPickPoints);
    group->addAction(ui->actionPickLines);
    group->addAction(ui->actionPickOutline);
    group->setExclusive(true);
    connect(group, SIGNAL(triggered(QAction*)),this,SLOT(updatePickModePicker()));
    updatePickModeActions();

    updateSliceConnections();

    ui->volumeView->setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Inline, 0});
}

SliceViewer::~SliceViewer()
{
    delete ui;
}

void SliceViewer::setProject(AVOProject * p){
    m_project=p;
    if( !p) return;

    QTransform xy_to_ilxl, ilxl_to_xy;
    p->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);
    ui->volumeView->setTransforms(xy_to_ilxl, ilxl_to_xy);

    updateFlattenHorizons();
    updateMuteHorizons();

    if(mVolumeItemsDialog){
        delete mVolumeItemsDialog;
        mVolumeItemsDialog=nullptr;
    }

    if(mHorizonItemsDialog){
        delete mHorizonItemsDialog;
        mHorizonItemsDialog=nullptr;
    }

    if(mWellItemsDialog){
        delete mWellItemsDialog;
        mWellItemsDialog=nullptr;
    }

    if(mMarkerItemsDialog){
        delete mMarkerItemsDialog;
        mMarkerItemsDialog=nullptr;
    }

    if(mTableItemsDialog){
        delete mTableItemsDialog;
        mTableItemsDialog=nullptr;
    }
}

void SliceViewer::addVolume(QString name){

    if( !m_project ) return;

    auto volume=m_project->loadVolume(name, true);
    if( !volume) return;

    VolumeItem* vitem=new VolumeItem(this);
    vitem->setName(name);
    auto r=volume->valueRange();
    vitem->colorTable()->setRange(r);
    vitem->setVolume(volume);
    HistogramCreator creator;
    auto histogram=creator.createHistogram(std::begin(*vitem->volume()), std::end(*vitem->volume()),
                                                     vitem->volume()->NULL_VALUE,256);
    vitem->setHistogram(std::make_shared<Histogram>(histogram));
    ui->volumeView->volumeItemModel()->add(vitem);

    if( ui->volumeView->volumeItemModel()->size()==1){
        ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Inline, volume->bounds().i1()});
    }

 }

void SliceViewer::cbSlice_currentIndexChanged(QString)
{
    if(m_noupdate) return;
    auto sliceType=static_cast<VolumeView::SliceType>(m_cbSlice->currentData().toInt());
    auto v=m_sbSlice->value();
    v=ui->volumeView->adjustToVolume(sliceType,v);
    ui->volumeView->setSlice(VolumeView::SliceDef{sliceType,v});
}

void SliceViewer::sbSlice_valueChanged(int arg1)
{
    if(m_noupdate) return;

    auto t=ui->volumeView->slice().type;
    ui->volumeView->setSlice(VolumeView::SliceDef{t, arg1});
}

void SliceViewer::setInlineSliceX(QPointF p){

    int il=static_cast<int>(std::round(p.x()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Inline, il});
}

void SliceViewer::setInlineSliceY(QPointF p){

    int il=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Inline, il});
}

void SliceViewer::setCrosslineSliceX(QPointF p){

    int xl=static_cast<int>(std::round(p.x()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Crossline, xl});
}

void SliceViewer::setCrosslineSliceY(QPointF p){

    int xl=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Crossline, xl});
}

void SliceViewer::setZSlice(QPointF p){

    int z=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Z, z});
}

void SliceViewer::setFlattenHorizon(QString name){

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

void SliceViewer::setMuteHorizon(QString name){

    if( !m_project) return;

    std::shared_ptr<Grid2D<float>> hrz;
    if( !name.isEmpty() && name!=NO_HORIZON ){
        hrz=m_project->loadGrid(GridType::Horizon, name);
        if( !hrz ){
            QMessageBox::critical(this, tr("Load Horizon"), tr("Loading Horizon \"%1\" failed!").arg(name), QMessageBox::Ok);
        }
    }
    ui->volumeView->setMuteHorizon(hrz);
}

void SliceViewer::updateSliceConnections(){

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
        if(ui->volumeView->displayOptions().inlineOrientation()==Qt::Vertical){
             connect( hsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setInlineSliceX(QPointF)) );
             connect( vsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setCrosslineSliceY(QPointF)) );
        }
        else{
            connect( hsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setCrosslineSliceX(QPointF)) );
            connect( vsv, SIGNAL(pointSelected(QPointF)), this, SLOT(setInlineSliceY(QPointF)) );
        }
         break;
    }

}

void SliceViewer::setupMouseModes(){

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


void SliceViewer::setupSliceToolBar(){
    m_sliceToolBar=new QToolBar( "Slice", this);
    auto widget=new QWidget(this);
    m_cbSlice=new QComboBox;
    m_cbSlice->setToolTip(tr("Slice type"));
    m_cbSlice->addItem( tr("Inline"), static_cast<int>(VolumeView::SliceType::Inline));
    m_cbSlice->addItem( tr("Crossline"), static_cast<int>(VolumeView::SliceType::Crossline));
    m_cbSlice->addItem( tr("depth/time"), static_cast<int>(VolumeView::SliceType::Z));
    m_cbSlice->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_sbSlice=new ReverseSpinBox;
    m_sbSlice->setToolTip(tr("Sclice value"));
    m_sbSlice->setReverse(false);
    auto layout=new QHBoxLayout;
    layout->addWidget(m_cbSlice);
    layout->addWidget(m_sbSlice);
    widget->setLayout(layout);
    m_sliceToolBar->addWidget(widget);
    addToolBar(m_sliceToolBar);
    connect(m_cbSlice, SIGNAL(currentIndexChanged(QString)), this, SLOT(cbSlice_currentIndexChanged(QString)) );
    connect(m_sbSlice, SIGNAL(valueChanged(int)), this, SLOT(sbSlice_valueChanged(int)) );
}

void SliceViewer::setupFlattenToolBar(){
    m_flattenToolBar=new QToolBar( "Flatten", this);
    auto widget=new QWidget(this);
    m_cbFlattenHorizon=new QComboBox;
    m_cbFlattenHorizon->setToolTip("Flatten on horizon");
    auto layout=new QHBoxLayout;
    layout->addWidget(new QLabel("Flatten on"));
    layout->addWidget(m_cbFlattenHorizon);
    widget->setLayout(layout);
    m_flattenToolBar->addWidget(widget);
    addToolBar(m_flattenToolBar);
    connect(m_cbFlattenHorizon, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFlattenHorizon(QString)));
}

void SliceViewer::setupMuteToolBar(){
    m_muteToolBar=new QToolBar( "Mute", this);
    auto widget=new QWidget(this);
    m_cbMuteHorizon=new QComboBox;
    m_cbMuteHorizon->setToolTip("Flatten on horizon");
    auto layout=new QHBoxLayout;
    layout->addWidget(new QLabel("Top mute"));
    layout->addWidget(m_cbMuteHorizon);
    widget->setLayout(layout);
    m_muteToolBar->addWidget(widget);
    addToolBar(m_muteToolBar);
    connect(m_cbMuteHorizon, SIGNAL(currentIndexChanged(QString)), this, SLOT(setMuteHorizon(QString)));
}

void SliceViewer::updateFlattenHorizons(){

    m_cbFlattenHorizon->clear();
    m_cbFlattenHorizon->addItem(NO_HORIZON);

    if( !m_project) return;

    for( auto name:m_project->gridList(GridType::Horizon)){
        m_cbFlattenHorizon->addItem(name);
    }
}

void SliceViewer::updateMuteHorizons(){

    m_cbMuteHorizon->clear();
    m_cbMuteHorizon->addItem(NO_HORIZON);

    if( !m_project) return;

    for( auto name:m_project->gridList(GridType::Horizon)){
        m_cbMuteHorizon->addItem(name);
    }
}

void SliceViewer::onVolumesChanged(){

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
        if(!vitem->histogram()) continue;

        auto colorbar = new ColorBarWidget(this);//  SmartColorBarWidget(this);
        colorbar->setMinimumSize(150,300);
        colorbar->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(colorbar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(colorBarContextMenuRequested(QPoint)));
        auto mdiColorbar=ui->mdiArea->addSubWindow(colorbar, Qt::WindowMinMaxButtonsHint | Qt::WindowTitleHint );
        mdiColorbar->setWindowIcon( QIcon(QPixmap(1,1)) );  // no icon
        mdiColorbar->setWindowTitle(name);
        auto colortable=vitem->colorTable();
        //colorbar->setItemName(vitem->name());
        colorbar->setLabel(vitem->name());  // XXX
        colorbar->setColorTable(colortable);
        //colorbar->setHistogram(*vitem->histogram());
        colorbar->show();
        m_mdiColorbars.insert(name, mdiColorbar);
    }

    // show attribute volume colorbars, hide seismic colorbars
    auto model=ui->volumeView->volumeItemModel();
    for( int i=0; i<model->size(); i++){
        auto vitem=dynamic_cast<VolumeItem*>(model->at(i));
        if(!vitem)continue;
        auto name=vitem->name();
        if(!m_mdiColorbars.contains(name)) continue;     // should never happen
        m_mdiColorbars.value(name)->setVisible(vitem->style()==VolumeItem::Style::ATTRIBUTE);
     }
}

void SliceViewer::onSliceChanged(VolumeView::SliceDef d){

    if( m_noupdate) return;

    m_noupdate=true;
    m_cbSlice->setCurrentIndex(m_cbSlice->findData(static_cast<int>(d.type)));

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

void SliceViewer::onMouseOver(QPointF p){

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
        if(ui->volumeView->displayOptions().inlineOrientation()==Qt::Horizontal){
            std::swap(il,xl);
        }
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



void SliceViewer::on_actionSetup_Volumes_triggered()
{
    Q_ASSERT(m_project);

    if(!mVolumeItemsDialog){
        mVolumeItemsDialog=new VolumeItemsDialog(m_project, ui->volumeView->volumeItemModel(), this );
        mVolumeItemsDialog->setWindowTitle("Setup volumes");
    }
    mVolumeItemsDialog->show();
}



void SliceViewer::on_actionSetup_Horizons_triggered()
{
    Q_ASSERT(m_project);

    if(!mHorizonItemsDialog){
        mHorizonItemsDialog=new HorizonItemsDialog(m_project, ui->volumeView->horizonItemModel(), this);
        mHorizonItemsDialog->setWindowTitle("Setup horizons");
    }
    mHorizonItemsDialog->show();
}

void SliceViewer::on_actionSetup_Wells_triggered()
{
    Q_ASSERT(m_project);

    if(!mWellItemsDialog){
        mWellItemsDialog=new WellItemsDialog(m_project, ui->volumeView->wellItemModel(), this);
        mWellItemsDialog->setWindowTitle("Setup Wells");
    }
    mWellItemsDialog->show();
}

void SliceViewer::on_actionSetup_Tops_triggered()
{
    Q_ASSERT(m_project);

    if(!mMarkerItemsDialog){
        mMarkerItemsDialog=new MarkerItemsDialog(m_project, ui->volumeView->markerItemModel(), this);
        mMarkerItemsDialog->setWindowTitle("Setup Markers");
    }
    mMarkerItemsDialog->show();
}

void SliceViewer::on_actionSetup_Tables_triggered()
{
    Q_ASSERT(m_project);

    if(!mTableItemsDialog){
        mTableItemsDialog=new TableItemsDialog(m_project, ui->volumeView->tableItemModel(), this);
        mTableItemsDialog->setWindowTitle("Setup Tables");
    }
    mTableItemsDialog->show();
}


void SliceViewer::on_actionDisplay_Options_triggered()
{
    DisplayOptionsDialog dlg;
    dlg.setWindowTitle("Configure display options");
    dlg.setDisplayOptions(ui->volumeView->displayOptions());
    if(dlg.exec()==QDialog::Accepted){
        ui->volumeView->setDisplayOptions(dlg.displayOptions());
    }
}

void SliceViewer::on_action_Player_triggered()
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

bool SliceViewer::canDiscardPicks(){
    if( !ui->volumeView->picker()->isDirty()) return true;
    auto reply = QMessageBox::warning(this, tr("Discard Picks"),
                                      tr("Picks have unsaved changes. Continuing will delete them. Continue?"),
                                        QMessageBox::Yes | QMessageBox::No );
    return reply == QMessageBox::Yes;
}

void SliceViewer::on_action_New_Picks_triggered()
{
    if( !canDiscardPicks()) return;
    auto picker=ui->volumeView->picker();
    picker->newPicks();

    m_picksName.clear();
}

void SliceViewer::on_action_Load_Picks_triggered()
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

void SliceViewer::on_action_Save_Picks_triggered()
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

void SliceViewer::on_actionSave_As_Picks_triggered()
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


void SliceViewer::closeEvent(QCloseEvent *event)
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

void SliceViewer::keyPressEvent(QKeyEvent * event){

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

void SliceViewer::updatePickModeActions(){
    if(!ui->volumeView->picker()) return;
    switch(ui->volumeView->picker()->pickMode()){
    case VolumePicker::PickMode::Points:
        ui->actionPickPoints->setChecked(true);
        break;
    case VolumePicker::PickMode::Lines:
        ui->actionPickLines->setChecked(true);
        break;
    case VolumePicker::PickMode::Outline:
        ui->actionPickOutline->setChecked(true);
        break;
    }
}

void SliceViewer::updatePickModePicker(){
    if(!ui->volumeView->picker()) return;
    if(ui->actionPickPoints->isChecked()){
        ui->volumeView->picker()->setPickMode(VolumePicker::PickMode::Points);
    }
    else if(ui->actionPickLines->isChecked()){
        ui->volumeView->picker()->setPickMode(VolumePicker::PickMode::Lines);
    }
    else if(ui->actionPickOutline->isChecked()){
        ui->volumeView->picker()->setPickMode(VolumePicker::PickMode::Outline);
    }
}

void SliceViewer::on_mdiArea_customContextMenuRequested(const QPoint &pos)
{
    QMenu popup;
    popup.addAction("tiles");
    popup.addAction("cascade");
    auto selected = popup.exec(ui->mdiArea->mapToGlobal(pos));
    if(!selected) return;
    if( selected->text()=="tiles"){
        ui->mdiArea->tileSubWindows();
    }
    else if(selected->text()=="cascade"){
        ui->mdiArea->cascadeSubWindows();
    }
}

void SliceViewer::colorBarContextMenuRequested(const QPoint &pos){

    auto cbwidget=dynamic_cast<SmartColorBarWidget*>(sender());
    if(!cbwidget) return;
    ColorTable* ct=cbwidget->colorTable();
    if(!ct) return;

    QMenu menu;
    menu.addAction("Scaling");
    menu.addAction("Colors");
    menu.addAction("Setup volumes");
    auto res=menu.exec(cbwidget->mapToGlobal(pos));
    if(!res) return;
    if(res->text()=="Scaling"){
        auto histogram=cbwidget->histogram();
        HistogramRangeSelectionDialog displayRangeDialog;
        displayRangeDialog.setWindowTitle(QString("Scaling"));
        displayRangeDialog.setRange(ct->range());
        displayRangeDialog.setColorTable(ct);       // all connections with ble are made by dialog
        displayRangeDialog.setHistogram(histogram);
        displayRangeDialog.exec();

    }
    else if(res->text()=="Colors"){
        auto dlg=new ColorTableDialog(ct->colors(), this);
        dlg->setWindowTitle("Configure colors");
        if(dlg->exec()==QDialog::Accepted){
            ct->setColors(dlg->colors());
        }
        delete dlg;
    }
    else if(res->text()=="Setup volumes"){
        auto name=cbwidget->itemName();
        if(!mVolumeItemsDialog){
            mVolumeItemsDialog=new VolumeItemsDialog(m_project, ui->volumeView->volumeItemModel(), this );
            mVolumeItemsDialog->setWindowTitle("Setup volumes");
        }
        mVolumeItemsDialog->show();
        mVolumeItemsDialog->setCurrentItem(name);
    }
}


}

