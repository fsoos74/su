#include "mapviewer2.h"
#include "ui_mapviewer2.h"

#include<cmath>
#include<QMessageBox>
#include<QInputDialog>
#include<QToolBar>
#include<QMap>
#include<dynamicmousemodeselector.h>
#include<volumeitem.h>
#include<volumeitemconfigdialog.h>
#include<griditem.h>
#include<griditemconfigdialog.h>
#include<wellitem.h>
#include<wellitemconfigdialog.h>
#include<multiitemselectiondialog.h>
#include<histogramcolorbarwidget.h>
#include<selecttypeanditemdialog.h>


const int ITEM_TYPE_INDEX=0;
const int ITEM_NAME_INDEX=1;

QMap<MapViewer2::ItemType, int> ItemZMap{
    {MapViewer2::ItemType::Volume, 0},
    {MapViewer2::ItemType::HorizonGrid, 0},
    {MapViewer2::ItemType::AttributeGrid, 0},
    {MapViewer2::ItemType::OtherGrid, 0},
    {MapViewer2::ItemType::Well, 10}
};

MapViewer2::MapViewer2(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::MapViewer2)
{
    ui->setupUi(this);

    setupMouseModes();

    // prevent overlapping of tick labels, need to make this automatic
    ui->graphicsView->topRuler()->setMinimumPixelIncrement(100);

    ui->graphicsView->setAspectRatioMode(Qt::KeepAspectRatio);

    ui->graphicsView->setMouseTracking(true);  // also send mouse move events when no button is pressed
    connect( ui->graphicsView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );
    connect( ui->graphicsView, SIGNAL(mouseDoubleClick(QPointF)), this, SLOT(onMouseDoubleClick(QPointF)) );

    ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    updateScene();
}

MapViewer2::~MapViewer2()
{
    delete ui;
}


QStringList MapViewer2::itemList(MapViewer2::ItemType t){

    QStringList res;
    auto scene = ui->graphicsView->scene();
    if( !scene) return res;

    for( auto item : scene->items()){
        if( item->data(ITEM_TYPE_INDEX) == static_cast<int>(t)){
            res.push_back(item->data(ITEM_NAME_INDEX).toString());
        }
    }
    return res;
}


void MapViewer2::setupMouseModes(){

    DynamicMouseModeSelector* mm=new DynamicMouseModeSelector(this);
    connect( mm, SIGNAL(modeChanged(MouseMode)), ui->graphicsView, SLOT(setMouseMode(MouseMode)));
    mm->addMode(MouseMode::Explore);
    mm->addMode(MouseMode::Zoom);
    mm->addMode(MouseMode::Select);

    QToolBar* mouseToolBar=addToolBar("mouse toolbar");
    mouseToolBar->addWidget( mm);
    mouseToolBar->show();
}



void MapViewer2::setProject(AVOProject* project ){
    if( project==m_project) return;

    m_project=project;

    // adjust graphicsview geometry
    auto bbox=addMargins(m_project->geometry().bboxCoords());
    ui->graphicsView->setSceneRect( bbox );

    ui->graphicsView->setSceneRect(bbox);
    //ui->graphicsView->fitInView(bbox, ui->graphicsView->aspectRatioMode() );
    ui->graphicsView->zoomFitWindow();

    QTransform stf=ui->graphicsView->transform();
    // invert y-axis
    stf.scale( 1, -1);
    stf.translate( 0, -bbox.bottom() );

    ui->graphicsView->setTransform(stf);

    updateScene();
}


void MapViewer2::setWellRefDepth(qreal d){

    if( d==m_wellRefDepth) return;

    m_wellRefDepth=d;

    emit wellRefDepthChanged(d);
}

void MapViewer2::receivePoint( SelectionPoint, int ){

    return; // NOP
}

void MapViewer2::receivePoints( QVector<SelectionPoint>, int){

    return; // NOP
}

void MapViewer2::onMouseOver(QPointF scenePos){

   QString message=QString("x=%1,  y=%2").arg(scenePos.x()).arg(scenePos.y());

   QTransform ilxl_to_xy, xy_to_ilxl;
   if( m_project && m_project->geometry().computeTransforms( xy_to_ilxl, ilxl_to_xy ) ){
       auto ilxl=xy_to_ilxl.map(scenePos);
       int il=std::round(ilxl.x());
       int xl=std::round(ilxl.y());
       message+=QString(", iline*=%1, xline*=%2").arg(il).arg(xl);
       //std::cout<<message.toStdString()<<std::endl<<std::flush;
   }

   statusBar()->showMessage(message);
}

void MapViewer2::onMouseDoubleClick(QPointF p){

    std::cout<<"MapViewer2::onMouseDoubleClick"<<std::endl<<std::flush;

    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    QGraphicsItem* item=scene->itemAt(p, ui->graphicsView->viewportTransform()  );
    if( !item ) return;

    while( item->parentItem() ){
        item=item->parentItem();
    }
    ItemType t = static_cast<ItemType>(item->data(ITEM_TYPE_INDEX).toInt());

    switch(t){
    case ItemType::Volume:
        configVolumeItem(dynamic_cast<VolumeItem*>(item));     // dialog handles nullptr
        break;

    case ItemType::HorizonGrid:
    case ItemType::AttributeGrid:
    case ItemType::OtherGrid:
        configGridItem(dynamic_cast<GridItem*>(item));
        break;

    case ItemType::Well:
        configWellItem(dynamic_cast<WellItem*>(item));
        break;
    }
}

void MapViewer2::updateScene(){

    if( !m_project ) return;

    QGraphicsScene* scene=new QGraphicsScene(this);

    ui->graphicsView->setScene(scene);

    auto br = addMargins(m_project->geometry().bboxCoords());
    ui->graphicsView->setSceneRect( br );

    ui->graphicsView->zoomFitWindow();
}


void MapViewer2::on_actionSelect_Volumes_triggered()
{
    Q_ASSERT(m_project);

    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    QStringList avail=m_project->volumeList();
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Select Volumes"), tr("Select Volumes:"), avail, &ok, itemList(ItemType::Volume));
    if( !ok ) return;

    // first pass remove items
    for( auto item : scene->items()){


        if( item->data(ITEM_TYPE_INDEX).toInt()==static_cast<int>(ItemType::Volume)){

            QString name =item->data(ITEM_NAME_INDEX).toString();
            if( name.isEmpty()) continue;

            if( !names.contains(name)){ // this is not selected anymore
                scene->removeItem(item);
            }
            names.removeAll(name);  // this is already displayed and does not need to be added
            removeItemDockWidget(item);
        }
    }

    // now he only have names that need to be added
    // second pass add items
    for( auto name : names){

        auto v = m_project->loadVolume(name);

        if( !v ){
            QMessageBox::critical(this, tr("Add Volume"), QString("Loading volume \"%1\" failed!").arg(name) );
            break;
        }

        auto item = new VolumeItem( m_project);
        item->setVolume(v);
        item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::Volume));
        item->setData(ITEM_NAME_INDEX, name);
        item->setZValue(ItemZMap.value(ItemType::Volume, 0 ));
        scene->addItem(item);

        auto w = new HistogramColorBarWidget(this);
        w->setColorTable(item->colorTable());
        auto g=item->grid();
        w->setData( g->data(), g->size(), g->NULL_VALUE);
        w->setMinimumSize( 50, 200);
        addItemDockWidget(name, item, w);
    }
}


void MapViewer2::on_actionAdd_Grid_triggered()
{
    selectGrids(GridType::Other, ItemType::OtherGrid);
}

void MapViewer2::on_actionSelect_Horizons_triggered()
{
    selectGrids(GridType::Horizon, ItemType::HorizonGrid);
}

void MapViewer2::on_actionSelect_Attribute_Grids_triggered()
{
    selectGrids(GridType::Attribute, ItemType::AttributeGrid);
}


void MapViewer2::selectGrids(GridType gtype, ItemType itype){

    Q_ASSERT(m_project);

    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    QStringList avail=m_project->gridList(gtype);
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Select Grids"), tr("Select Grids:"), avail, &ok, itemList(itype));
    if( !ok ) return;

    // first pass remove items
    for( auto item : scene->items()){

        if( item->data(ITEM_TYPE_INDEX).toInt()==static_cast<int>(itype)){

            QString name =item->data(ITEM_NAME_INDEX).toString();
            if( name.isEmpty()) continue;

            if( !names.contains(name)){ // this is not selected anymore
                scene->removeItem(item);
                removeItemDockWidget(item);
            }
            names.removeAll(name);  // this is already displayed and does not need to be added
        }
    }

    // now we only have names that need to be added
    // second pass add items
    for( auto name : names){

        auto g = m_project->loadGrid(gtype, name);

        if( !g ){
            QMessageBox::critical(this, tr("Add Grid"), QString("Loading grid \"%1\" failed!").arg(name) );
            break;
        }

        auto item = new GridItem( m_project);
        item->setGrid(g);
        item->setData(ITEM_TYPE_INDEX, static_cast<int>(itype));
        item->setData(ITEM_NAME_INDEX, name);
        item->setZValue(ItemZMap.value(itype, 0 ));
        scene->addItem(item);

        auto w = new HistogramColorBarWidget(this);
        w->setColorTable(item->colorTable());
        w->setData( g->data(), g->size(), g->NULL_VALUE);
        w->setMinimumSize( 50, 200);
        addItemDockWidget(name, item, w);
    }
}


void MapViewer2::on_actionAdd_Wells_triggered()
{
    Q_ASSERT(m_project);

    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    auto avail = m_project->wellList();
    bool ok=false;
    auto names=MultiItemSelectionDialog::getItems(nullptr, tr("Select Wells"), tr("Select Wells:"), avail, &ok, itemList(ItemType::Well));
    if( !ok ) return;

    // first pass remove items
    for( auto item : scene->items()){

        if( item->data(ITEM_TYPE_INDEX).toInt()==static_cast<int>(ItemType::Well)){

            QString name =item->data(ITEM_NAME_INDEX).toString();
            if( name.isEmpty()) continue;
            if( !names.contains(name)){ // this is not selected anymore
                scene->removeItem(item);
            }
            names.removeAll(name);  // this is already displayed and does not need to be added
        }
    }

    // now he only have names that need to be added
    // second pass add items
    for( auto name : names){

        auto info=m_project->getWellInfo(name);
        auto path=m_project->loadWellPath(name);
        auto item=new WellItem;

        item->setInfo(info);
        item->setPath(path);
        //item->setPos(info.x(), info.y());
        item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::Well) );
        item->setData(ITEM_NAME_INDEX, name);
        item->setZValue(ItemZMap.value(ItemType::Well, 0 ));

        item->setSize(m_defaultWellItem.size());
        item->setFont(m_defaultWellItem.font());
        item->setPen(m_defaultWellItem.pen());
        item->setBrush(m_defaultWellItem.brush());

        connect( this, SIGNAL(wellRefDepthChanged(qreal)), item, SLOT(setRefDepth(qreal)) );

        scene->addItem(item);
    }

   }



void MapViewer2::on_actionSetup_Grids_triggered()
{
    /*
    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    auto avail = itemList(ItemType::Grid);
    bool ok=false;
    auto edit = QInputDialog::getItem( nullptr, tr("Configure Grid Item"), tr("Select Grid:"), avail, 0, false, &ok);
    if( !ok || edit.isEmpty() ) return;

    auto gitem=dynamic_cast<GridItem*>(findItem(ItemType::Grid, edit));
    if( gitem) configGridItem(gitem);
    */
}



void MapViewer2::on_actionSetup_Volume_triggered()
{
    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    auto avail = itemList(ItemType::Volume);
    bool ok=false;
    auto edit = QInputDialog::getItem( nullptr, tr("Configure Volume Item"), tr("Select Volume:"), avail, 0, false, &ok);
    if( !ok || edit.isEmpty() ) return;

    auto item=dynamic_cast<VolumeItem*>(findItem(ItemType::Volume, edit));
    if( item) configVolumeItem(item);
}


void MapViewer2::on_actionSetup_Wells_triggered()
{
    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    WellItemConfigDialog dlg;
    dlg.setWindowTitle(tr("Setup Well Display"));
    dlg.setLabelType(static_cast<int>(m_defaultWellItem.labelType()));
    dlg.setLabelSize(m_defaultWellItem.font().pointSize());//  pixelSize());
    dlg.setItemSize(m_defaultWellItem.size());
    dlg.setPenColor(m_defaultWellItem.pen().color());
    dlg.setBrushColor(m_defaultWellItem.brush().color());

    if( dlg.exec()!=QDialog::Accepted) return;

    m_defaultWellItem.setLabelType(static_cast<WellItem::LabelType>(dlg.labelType()));
    QFont f=m_defaultWellItem.font();
    f.setPointSize(dlg.labelSize());
    m_defaultWellItem.setFont(f);
    m_defaultWellItem.setSize( dlg.itemSize() );

    QPen pen=m_defaultWellItem.pen();
    pen.setColor(dlg.penColor());
    m_defaultWellItem.setPen(pen);

    QBrush brush = m_defaultWellItem.brush();
    brush.setColor(dlg.brushColor());
    m_defaultWellItem.setBrush(brush);


    // now assign new options to all well items

    for( auto item : scene->items()){

        if( item->data(ITEM_TYPE_INDEX).toInt()==static_cast<int>(ItemType::Well)){

            auto wi=dynamic_cast<WellItem*>(item);
            if( !wi ) continue;  // better save than sorry

            wi->setLabelType(m_defaultWellItem.labelType());
            wi->setSize(m_defaultWellItem.size());
            wi->setFont(m_defaultWellItem.font());
            wi->setPen(m_defaultWellItem.pen());
            wi->setBrush(m_defaultWellItem.brush());
        }
    }
}


void MapViewer2::on_actionSet_Well_Reference_Depth_triggered()
{
    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    bool ok=false;
    auto d = QInputDialog::getDouble(nullptr, tr("Well Reference Depth"), tr("Select Depth:"), m_wellRefDepth, -999999, 999999, 1, &ok);
    if( !ok ) return;

    setWellRefDepth(d);
}




void MapViewer2::configWellItem(WellItem * item){

    if( ! item) return;

    bool ok=false;
    QString text=QInputDialog::getText(nullptr, tr("Change Well Label"), tr("Enter Label Text:"), QLineEdit::Normal, item->text(), &ok);
    if(!text.isNull() && ok ){
        item->setText(text);
    }
}

void MapViewer2::configGridItem(GridItem * item){

    if( !item ) return;
    if( !item->grid()) return;
    auto histogram=createHistogram( std::begin(*(item->grid())), std::end(*(item->grid())), item->grid()->NULL_VALUE, 100 );
    GridItemConfigDialog* dlg=new GridItemConfigDialog;
    dlg->setWindowTitle(tr("Configure Grid Item"));
    dlg->setColorTable(item->colorTable());      // all updates of scling and colors done through colortable
    dlg->setHistogram(histogram);
    dlg->setShowLabels(item->showLabels());
    dlg->setShowMesh(item->showMesh());
    dlg->setInlineIncrement(item->inlineIncrement());
    dlg->setCrosslineIncrement(item->crosslineIncrement());
    connect(dlg, SIGNAL(showLabelsChanged(bool)), item, SLOT( setShowLabels(bool)));
    connect(dlg, SIGNAL(showMeshChanged(bool)), item, SLOT( setShowMesh(bool)));
    connect(dlg, SIGNAL(inlineIncrementChanged(int)), item, SLOT(setInlineIncrement(int)) );
    connect(dlg, SIGNAL(crosslineIncrementChanged(int)), item, SLOT(setCrosslineIncrement(int)) );

    if( dlg->exec()==QDialog::Accepted){

    }
    else{

    }

    delete dlg;
}


void MapViewer2::configVolumeItem(VolumeItem * item){

    if( !item ) return;
    if( !item->grid()) return;
    auto histogram=createHistogram( std::begin(*(item->volume())), std::end(*(item->volume())), item->volume()->NULL_VALUE, 100 );
    auto dlg=new VolumeItemConfigDialog;
    dlg->setWindowTitle(tr("Configure Volume Item"));
    dlg->setColorTable(item->colorTable());      // all updates of scling and colors done through colortable
    dlg->setHistogram(histogram);
    dlg->setShowLabels(item->showLabels());
    dlg->setShowMesh(item->showMesh());
    dlg->setInlineIncrement(item->inlineIncrement());
    dlg->setCrosslineIncrement(item->crosslineIncrement());
    auto bounds=item->volume()->bounds();
    dlg->setTimeRange(static_cast<int>(1000*bounds.ft()), static_cast<int>(1000*bounds.lt()));      // sec -> msec
    dlg->setTimeIncrement(static_cast<int>(1000*bounds.dt()));                                      // sec -> msec
    dlg->setTime(static_cast<int>(1000*item->time()));                                                   //  sec -> msec
    connect(dlg, SIGNAL(showLabelsChanged(bool)), item, SLOT( setShowLabels(bool)));
    connect(dlg, SIGNAL(showMeshChanged(bool)), item, SLOT( setShowMesh(bool)));
    connect(dlg, SIGNAL(inlineIncrementChanged(int)), item, SLOT(setInlineIncrement(int)) );
    connect(dlg, SIGNAL(crosslineIncrementChanged(int)), item, SLOT(setCrosslineIncrement(int)) );
    connect(dlg, SIGNAL(timeChanged(double)), item, SLOT(setTime(double)) );
    if( dlg->exec()==QDialog::Accepted){

    }
    else{

    }

    delete dlg;
}

void MapViewer2::addItemDockWidget( QString title, QGraphicsItem* item, QWidget* w){

    auto dw = new QDockWidget(title, this);
    dw->setAllowedAreas(Qt::LeftDockWidgetArea);
    dw->setContentsMargins(10, 5, 10, 5);
    dw->setWidget(w);
    dw->setFeatures(QDockWidget::DockWidgetVerticalTitleBar | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);   // and nothing else
    addDockWidget(Qt::LeftDockWidgetArea, dw);
    m_dockWidgets.insert(item, dw);
}

void MapViewer2::removeItemDockWidget( QGraphicsItem* item){

    std::cout<<"MapViewer2::removeItemDockWidget "<<item<<std::endl<<std::flush;
    if( ! m_dockWidgets.contains(item) ) return;
    auto dw = m_dockWidgets.value(item);
    removeDockWidget(dw);
    delete dw;
    m_dockWidgets.remove(item);
    std::cout<<"FINISH"<<std::endl<<std::flush;
}

QGraphicsItem* MapViewer2::findItem(ItemType type, QString name)
{
    auto scene = ui->graphicsView->scene();
    if( !scene ) return nullptr;

    for( auto item : scene->items()){

        if( item->data(ITEM_TYPE_INDEX).toInt()==static_cast<int>(type)){

            if( item->data(ITEM_NAME_INDEX).toString()==name ){

               return item;
            }
        }
    }

    return nullptr;
}

QRectF MapViewer2::addMargins(const QRectF & bbox){
    return bbox.marginsAdded( QMarginsF(bbox.width()/4, bbox.height()/4, bbox.width()/4, bbox.height()/4));
}






