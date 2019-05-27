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
#include <simplescalingdialog.h>
#include <multiinputdialog.h>
#include <multiitemselectiondialog.h>
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

namespace sliceviewer {


const char* NO_HORIZON="NONE";

VolumeViewer2D::VolumeViewer2D(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VolumeViewer2D)
{
    ui->setupUi(this);

    //m_colorbarsLayout=new QVBoxLayout(ui->wdColortableArea);
    ui->splitter->setSizes(QList<int>{400,100});

    setupMouseModes();
    setupSliceToolBar();
    setupFlattenToolBar();

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
        ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Inline, volume->bounds().i1()});
    }

 }

void VolumeViewer2D::cbSlice_currentIndexChanged(QString)
{
    if(m_noupdate) return;
    auto sliceType=static_cast<VolumeView::SliceType>(m_cbSlice->currentData().toInt());
    auto v=m_sbSlice->value();
    v=ui->volumeView->adjustToVolume(sliceType,v);
    ui->volumeView->setSlice(VolumeView::SliceDef{sliceType,v});
}

void VolumeViewer2D::sbSlice_valueChanged(int arg1)
{
    if(m_noupdate) return;

    auto t=ui->volumeView->slice().type;
    ui->volumeView->setSlice(VolumeView::SliceDef{t, arg1});
}

void VolumeViewer2D::setInlineSliceX(QPointF p){

    int il=static_cast<int>(std::round(p.x()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Inline, il});
}

void VolumeViewer2D::setInlineSliceY(QPointF p){

    int il=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Inline, il});
}

void VolumeViewer2D::setCrosslineSliceX(QPointF p){

    int xl=static_cast<int>(std::round(p.x()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Crossline, xl});
}

void VolumeViewer2D::setCrosslineSliceY(QPointF p){

    int xl=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Crossline, xl});
}

void VolumeViewer2D::setZSlice(QPointF p){

    int z=static_cast<int>(std::round(p.y()));
    ui->volumeView->setSlice(VolumeView::SliceDef{VolumeView::SliceType::Z, z});
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

void VolumeViewer2D::setupFlattenToolBar(){
    m_flattenToolBar=new QToolBar( "Flatten", this);
    auto widget=new QWidget(this);
    m_cbHorizon=new QComboBox;
    m_cbHorizon->setToolTip("Flatten on horizon");
    auto layout=new QHBoxLayout;
    layout->addWidget(m_cbHorizon);
    widget->setLayout(layout);
    m_flattenToolBar->addWidget(widget);
    addToolBar(m_flattenToolBar);
    connect(m_cbHorizon, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFlattenHorizon(QString)));
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
        colorbar->setMinimumSize(150,300);
        colorbar->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(colorbar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(colorBarContextMenuRequested(QPoint)));
        //m_colorbarsLayout->addWidget(colorbar);
        auto mdiColorbar=ui->mdiArea->addSubWindow(colorbar, Qt::WindowMinMaxButtonsHint | Qt::WindowTitleHint );
        mdiColorbar->setWindowIcon( QIcon(QPixmap(1,1)) );  // no icon
        mdiColorbar->setWindowTitle(name);
        auto colortable=vitem->colorTable();
        colorbar->setColorTable(colortable);
        colorbar->setLabel("");//name);
        colorbar->setPrecision(3);
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

void VolumeViewer2D::onSliceChanged(VolumeView::SliceDef d){

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



void VolumeViewer2D::on_actionSetup_Volumes_triggered()
{
    Q_ASSERT(m_project);

    VolumeItemsDialog* dlg=new VolumeItemsDialog(m_project, ui->volumeView->volumeItemModel());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("Setup volumes");
    dlg->exec();
}



void VolumeViewer2D::on_actionSetup_Horizons_triggered()
{
    Q_ASSERT(m_project);

    HorizonItemsDialog* dlg=new HorizonItemsDialog(m_project, ui->volumeView->horizonItemModel());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("Setup horizons");
    dlg->exec();
}

void VolumeViewer2D::on_actionSetup_Wells_triggered()
{
    Q_ASSERT(m_project);

    WellItemsDialog* dlg=new WellItemsDialog(m_project, ui->volumeView->wellItemModel());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("Setup Wells");
    dlg->exec();
}

void VolumeViewer2D::on_actionSetup_Tops_triggered()
{
    Q_ASSERT(m_project);

    MarkerItemsDialog* dlg=new MarkerItemsDialog(m_project, ui->volumeView->markerItemModel());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("Setup Markers");
    dlg->exec();
}

void VolumeViewer2D::on_actionSetup_Tables_triggered()
{
    Q_ASSERT(m_project);

    TableItemsDialog* dlg=new TableItemsDialog(m_project, ui->volumeView->tableItemModel());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("Setup Tables");
    dlg->exec();
}


void VolumeViewer2D::on_actionDisplay_Options_triggered()
{
    DisplayOptionsDialog dlg;
    dlg.setWindowTitle("Configure display options");
    dlg.setDisplayOptions(ui->volumeView->displayOptions());
    if(dlg.exec()==QDialog::Accepted){
        ui->volumeView->setDisplayOptions(dlg.displayOptions());
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

void VolumeViewer2D::updatePickModeActions(){
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

void VolumeViewer2D::updatePickModePicker(){
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

void VolumeViewer2D::on_mdiArea_customContextMenuRequested(const QPoint &pos)
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

void VolumeViewer2D::colorBarContextMenuRequested(const QPoint &pos){

    ColorBarWidget* cbwidget=dynamic_cast<ColorBarWidget*>(sender());
    if(!cbwidget) return;
    ColorTable* ct=cbwidget->colorTable();
    if(!ct) return;

    QMenu menu;
    menu.addAction("Scaling");
    menu.addAction("Colors");
    auto res=menu.exec(cbwidget->mapToGlobal(pos));
    if(res->text()=="Scaling"){
        auto dlg=new SimpleScalingDialog(this);
        dlg->setWindowTitle("Configure scaling");
        dlg->setMinimum(ct->range().first);
        dlg->setMaximum(ct->range().second);
        dlg->setPower(ct->power());
        if(dlg->exec()==QDialog::Accepted){
            ct->setRange(dlg->minimum(), dlg->maximum());
            ct->setPower(dlg->power());
        }
        delete dlg;
    }
    else if(res->text()=="Colors"){
        auto dlg=new ColorTableDialog(ct->colors(), this);
        dlg->setWindowTitle("Configure colors");
        if(dlg->exec()==QDialog::Accepted){
            ct->setColors(dlg->colors());
        }
        delete dlg;
    }
}


}

