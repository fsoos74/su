#include "mapviewer2.h"
#include "ui_mapviewer2.h"
#include<cmath>
#include<QMessageBox>
#include<QInputDialog>
#include<QToolBar>
#include<QLabel>
#include<QMap>
#include<dynamicmousemodeselector.h>
#include<volumeitem.h>
#include<griditem.h>
#include<griditemconfigdialog.h>
#include<wellitem.h>
#include<multiitemselectiondialog.h>
#include<histogramcolorbarwidget.h>
#include<selecttypeanditemdialog.h>
#include<areaitem.h>
#include<rulergraphicsview.h>
#include<QVBoxLayout>
#include<QMouseEvent>
#include<optionsdialog.h>

const int ITEM_TYPE_INDEX=0;
const int ITEM_NAME_INDEX=1;

QMap<MapViewer2::ItemType, int> ItemZMap{
    {MapViewer2::ItemType::Area, -99},
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
    setupToolBarControls();
    // prevent overlapping of tick labels, need to make this automatic
    ui->graphicsView->topRuler()->setMinimumPixelIncrement(100);
    ui->graphicsView->setAspectRatioMode(Qt::KeepAspectRatio);
    ui->graphicsView->setMouseTracking(true);  // also send mouse move events when no button is pressed
    connect( ui->graphicsView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );
    connect( ui->graphicsView, SIGNAL(mouseDoubleClick(QPointF)), this, SLOT(onMouseDoubleClick(QPointF)) );

    connect( ui->actionShow_Project_Area, SIGNAL(toggled(bool)), this, SLOT(setShowProjectArea(bool)) );
    connect( this, SIGNAL(showProjectAreaChanged(bool)), ui->actionShow_Project_Area, SLOT(setChecked(bool)) );

    ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    ui->graphicsView->topRuler()->setAutoTickIncrement(true);
    ui->graphicsView->topRuler()->setTickIncrement(50000);
    ui->graphicsView->topRuler()->setTickMarkSize(0);
    ui->graphicsView->topRuler()->setSubTickMarkSize(0);
    ui->graphicsView->leftRuler()->setAutoTickIncrement(true);
    ui->graphicsView->leftRuler()->setTickMarkSize(0);
    ui->graphicsView->leftRuler()->setSubTickMarkSize(0);
    ui->graphicsView->topRuler()->installEventFilter(this);
    ui->graphicsView->leftRuler()->installEventFilter(this);
    QPen tickpen=QPen(Qt::lightGray,2);
    tickpen.setCosmetic(true);
    ui->graphicsView->setGridPen(tickpen);
    QPen subtickpen=QPen(Qt::lightGray,1);
    subtickpen.setCosmetic(true);
    ui->graphicsView->setSubGridPen(subtickpen);
    ui->graphicsView->setGridMode(RulerGraphicsView::GridMode::Background);// ::None);

    updateScene();

    ui->treeWidget->setHeaderHidden(true);
    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(updateItemsFromTree()));
    connect(this,SIGNAL(domainChanged(Domain)),this,SLOT(onDomainChanged(Domain)));

    m_defaultWellItem.setZValue(ItemZMap[ItemType::Well]);
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

    fillTree();

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


void MapViewer2::setShowProjectArea(bool on){

    if( on==m_showProjectArea) return;

    m_showProjectArea=on;

    //updateScene();

    auto item=findItem(ItemType::Area,"ProjectBounds");
    if(item){
        ui->graphicsView->scene()->removeItem(item);
    }

    if( m_showProjectArea){
        auto bbl=m_project->geometry().bboxLines();
        auto item=new AreaItem(m_project);
        item->setArea(bbl);
        item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::Area) );
        item->setData(ITEM_NAME_INDEX, "ProjectBounds");
        item->setZValue(ItemZMap.value(ItemType::Well, 0 ));
        item->setZValue(ItemZMap.value(ItemType::Area));
        ui->graphicsView->scene()->addItem(item);
    }

    ui->graphicsView->scene()->update();
}

void MapViewer2::setDomain(QString s){
    setDomain(toDomain(s));
}

void MapViewer2::setDomain(Domain d){
    if(d==m_domain) return;

    auto items=ui->graphicsView->scene()->items();
    for( auto item : items){
        auto type=static_cast<ItemType>(item->data(ITEM_TYPE_INDEX).toInt());
        if(type==ItemType::HorizonGrid || type==ItemType::Volume || type==ItemType::Well){
            ui->graphicsView->scene()->removeItem(item);
        }
        auto lw=m_legendWidgets.value(item);
        if(lw){
            delete lw;
            m_legendWidgets.remove(item);
        }
    }
    ui->graphicsView->scene()->update();  // remove leftovers

    m_domain=d;
    auto text=toQString(d);
    auto idx=m_cbDomain->findText(text);
    m_cbDomain->setCurrentIndex(idx);
    emit domainChanged(d);
    setWellReferenceDepth((d==Domain::Depth) ? m_sliceValue : 0);	// surface location for time slices
    fillTree();
}

void MapViewer2::setSliceValue(int x){
    if(x==m_sliceValue) return;
    m_sliceValue=x;
    emit sliceValueChanged(x);
    double d=(m_domain==Domain::Depth) ? x : 0;
    setWellReferenceDepth(d);
    m_sbSliceValue->setValue(x);
}

void MapViewer2::setWellReferenceDepth(double d){
    if(d==m_wellReferenceDepth) return;
    m_wellReferenceDepth=d;
    emit wellReferenceDepthChanged(d);
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

    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    QGraphicsItem* item=scene->itemAt(p, ui->graphicsView->viewportTransform()  );
    if( !item ) return;

    while( item->parentItem() ){
        item=item->parentItem();
    }
    ItemType t = static_cast<ItemType>(item->data(ITEM_TYPE_INDEX).toInt());

    switch(t){
    case ItemType::Area:
        configAreaItem(dynamic_cast<AreaItem*>(item));
        break;
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
    default:
        break;
    }
}

bool MapViewer2::eventFilter(QObject *watched, QEvent *event){

    if(event->type()==QEvent::MouseButtonDblClick){
        std::cout<<"MV2: DBLOCLCK"<<std::endl<<std::flush;
        auto mdisw=dynamic_cast<QMdiSubWindow*>(watched);
        if(mdisw){
            if(!m_legendWidgets.values().contains(mdisw)) return false;
            QGraphicsItem* item=m_legendWidgets.key(mdisw);
            auto vitem=dynamic_cast<VolumeItem*>(item);
            if(vitem){
                configVolumeItem(vitem);
                return true;
            }
            auto gitem=dynamic_cast<GridItem*>(item);
            if(gitem){
                configGridItem(gitem);
                return true;
            }
        }
        auto gvruler=dynamic_cast<GVRuler*>(watched);
        if(gvruler){
            configAxis(gvruler);
            return true;
        }
        return false;
    }
    return false;
}

void MapViewer2::updateScene(){

    if( !m_project ) return;

    QGraphicsScene* scene=new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    auto br = addMargins(m_project->geometry().bboxCoords());
    ui->graphicsView->setSceneRect( br );

    ui->graphicsView->zoomFitWindow();

    if( m_showProjectArea){
        auto bbl=m_project->geometry().bboxLines();
        auto item=new AreaItem(m_project);
        item->setArea(bbl);
        item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::Area) );
        item->setData(ITEM_NAME_INDEX, "ProjectBounds");
        item->setZValue(ItemZMap.value(ItemType::Well, 0 ));
        item->setZValue(ItemZMap.value(ItemType::Area));
        scene->addItem(item);
    }
}


void MapViewer2::updateItemsFromTree(){

    static bool ignoreit=false;
    if(ignoreit) return;
    ignoreit=true;
    // works but does not look good: ui->treeWidget->setEnabled(false);	// prevent new events being triggered while this is running

    bool redraw=false;	// need this to trigger full redraw when items were removed.
                        // this ensures leftovers (outside of items bbox) are removed

    // all items are in tree!!
    // volumes
    auto volumes=ui->treeWidget->items("Volumes");
    for(auto name : volumes){
        auto item=findItem(ItemType::Volume, name);
        auto checked=ui->treeWidget->isChecked("Volumes",name);
        if(checked){
            if(!item){	// need to create new item
                addVolumeItem(name);
            }
        }
        else{
            if( item ){	// need to remove item
                removeItemLegendWidget(item);
                ui->graphicsView->scene()->removeItem(item);
                redraw=true;
            }
        }
    }

    // horizons
    auto horizons=ui->treeWidget->items("Horizons");
    for(auto name : horizons){
        auto item=findItem(ItemType::HorizonGrid, name);
        auto checked=ui->treeWidget->isChecked("Horizons",name);
        if(checked){
            if(!item){	// need to create new item
                addHorizonItem(name);
            }
        }
        else{
            if( item ){	// need to remove item
                removeItemLegendWidget(item);
                ui->graphicsView->scene()->removeItem(item);
                redraw=true;
            }
        }
    }

    // other grids
    auto ogrids=ui->treeWidget->items("Other Grids");
    for(auto name : ogrids){
        auto item=findItem(ItemType::OtherGrid, name);
        auto checked=ui->treeWidget->isChecked("Other Grids",name);
        if(checked){
            if(!item){	// need to create new item
                addOtherGridItem(name);
            }
        }
        else{
            if( item ){	// need to remove item
                removeItemLegendWidget(item);
                ui->graphicsView->scene()->removeItem(item);
                redraw=true;
            }
        }
    }

    // Attribute grids
    auto agrids=ui->treeWidget->items("Attribute Grids");
    for(auto name : agrids){
        auto item=findItem(ItemType::AttributeGrid, name);
        auto checked=ui->treeWidget->isChecked("Attribute Grids",name);
        if(checked){
            if(!item){	// need to create new item
                addAttributeGridItem(name);
            }
        }
        else{
            if( item ){	// need to remove item
                removeItemLegendWidget(item);
                ui->graphicsView->scene()->removeItem(item);
                redraw=true;
            }
        }
    }

    // wells
    auto wells=ui->treeWidget->items("Wells");
    for(auto name : wells){
        auto item=findItem(ItemType::Well, name);
        auto checked=ui->treeWidget->isChecked("Wells",name);
        if(checked){
            if(!item){	// need to create new item
                addWellItem(name);
            }
        }
        else{
            if( item ){	// need to remove item
                ui->graphicsView->scene()->removeItem(item);
                redraw=true;
            }
        }
    }

    if( redraw ){
        ui->graphicsView->scene()->update();   // remove remains of deleted items
    }

    //ui->treeWidget->setEnabled(true);
    ignoreit=false;
}

void MapViewer2::updateTreeFromItems(){
    static bool ignoreit=false;
    if(ignoreit)return;
    ignoreit=true;

    QSet<QString> shorizons;
    QSet<QString> sothergrids;
    QSet<QString> sattributegrids;
    QSet<QString> svolumes;
    QSet<QString> swells;
    auto items=ui->graphicsView->items();
    for( auto item : items){
        auto type=static_cast<ItemType>(item->data(ITEM_TYPE_INDEX).toInt());
        auto name=item->data(ITEM_NAME_INDEX).toString();
        switch(type){
        case ItemType::HorizonGrid: shorizons.insert(name); break;
        case ItemType::OtherGrid: sothergrids.insert(name); break;
        case ItemType::AttributeGrid: sattributegrids.insert(name); break;
        case ItemType::Volume: svolumes.insert(name);break;
        case ItemType::Well: swells.insert(name); break;
        default: break;
        }

    }

    // horizons
    auto horizons=ui->treeWidget->items("Horizons");
    for(auto name : horizons){
        auto checked=shorizons.contains(name);
        ui->treeWidget->setChecked("Horizons",name,checked);
    }

    // other grids
    auto ogrids=ui->treeWidget->items("Other Grids");
    for(auto name : ogrids){
        auto checked=sothergrids.contains(name);
        ui->treeWidget->setChecked("Other Grids",name,checked);
    }

    // attribute grids
    auto agrids=ui->treeWidget->items("Attribute Grids");
    for(auto name : agrids){
        auto checked=sattributegrids.contains(name);
        ui->treeWidget->setChecked("Attribute Grids",name,checked);
    }

    // volumes
    auto volumes=ui->treeWidget->items("Volumes");
    for(auto name : volumes){
        auto checked=svolumes.contains(name);
        ui->treeWidget->setChecked("Volumes",name,checked);
    }

    // horizons
    auto wells=ui->treeWidget->items("Wells");
    for(auto name : wells){
        auto checked=swells.contains(name);
        ui->treeWidget->setChecked("Wells",name,checked);
    }
    ignoreit=false;
}

void MapViewer2::addVolumeItem(QString name){
    auto v = m_project->loadVolume(name);

    if( !v ){
        QMessageBox::critical(this, tr("Add Volume"), QString("Loading volume \"%1\" failed!").arg(name) );
        return;
    }

    auto item = new VolumeItem( m_project);
    item->setVolume(v);
    item->setLabel(name);
    item->setShowLineLabels(false);
    item->setShowMesh(false);
    item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::Volume));
    item->setData(ITEM_NAME_INDEX, name);
    item->setZValue(ItemZMap.value(ItemType::Volume, 0 ));
    item->setSliceValue(sliceValue());
    connect( this, SIGNAL(sliceValueChanged(int)), item, SLOT(setSliceValue(int)) );
    ui->graphicsView->scene()->addItem(item);

    auto w = new HistogramColorBarWidget(this);
    w->setOrientation(Qt::Horizontal);
    w->setColorTable(item->colorTable());
    auto g=item->grid();
    w->setData( g->data(), g->size(), g->NULL_VALUE);
    connect(item,SIGNAL(dataChanged(float*,size_t,float)),w,SLOT(setData(float*,size_t,float)));
    //connect(this, SIGNAL(sliceValueChanged(int)), w, SLOT(updateHistogram()));
    addItemLegendWidget(item,w,name);
}

void MapViewer2::addHorizonItem(QString name){
    auto g = m_project->loadGrid(GridType::Horizon, name);

    if( !g ){
        QMessageBox::critical(this, tr("Add Grid"), QString("Loading grid \"%1\" failed!").arg(name) );
        return;
    }

    auto item = new GridItem( m_project);
    item->setLabel(name);
    item->setGrid(g);
    item->setShowLineLabels(false);
    item->setShowMesh(false);
    item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::HorizonGrid));
    item->setData(ITEM_NAME_INDEX, name);
    item->setZValue(ItemZMap.value(ItemType::HorizonGrid, 0 ) );
    ui->graphicsView->scene()->addItem(item);

    auto w = new HistogramColorBarWidget;
    w->setOrientation(Qt::Horizontal);
    w->setColorTable(item->colorTable());
    w->setData( g->data(), g->size(), g->NULL_VALUE);
    w->setFixedSize( 200, 200);
    connect(item,SIGNAL(dataChanged(float*,size_t,float)),w,SLOT(setData(float*,size_t,float)));
    addItemLegendWidget(item,w,name);
}

void MapViewer2::addOtherGridItem(QString name){
    auto g = m_project->loadGrid(GridType::Other, name);

    if( !g ){
        QMessageBox::critical(this, tr("Add Grid"), QString("Loading grid \"%1\" failed!").arg(name) );
        return;
    }

    auto item = new GridItem( m_project);
    item->setLabel(name);
    item->setGrid(g);
    item->setShowLineLabels(false);
    item->setShowMesh(false);
    item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::OtherGrid));
    item->setData(ITEM_NAME_INDEX, name);
    item->setZValue(ItemZMap.value(ItemType::OtherGrid, 0 ) );
    ui->graphicsView->scene()->addItem(item);

    auto w = new HistogramColorBarWidget;
    w->setOrientation(Qt::Horizontal);
    w->setColorTable(item->colorTable());
    w->setData( g->data(), g->size(), g->NULL_VALUE);
    w->setFixedSize( 200, 200);
    connect(item,SIGNAL(dataChanged(float*,size_t,float)),w,SLOT(setData(float*,size_t,float)));
    addItemLegendWidget(item,w,name);
}

void MapViewer2::addAttributeGridItem(QString name){
    auto g = m_project->loadGrid(GridType::Attribute, name);

    if( !g ){
        QMessageBox::critical(this, tr("Add Grid"), QString("Loading grid \"%1\" failed!").arg(name) );
        return;
    }

    auto item = new GridItem( m_project);
    item->setLabel(name);
    item->setGrid(g);
    item->setShowLineLabels(false);
    item->setShowMesh(false);
    item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::AttributeGrid));
    item->setData(ITEM_NAME_INDEX, name);
    item->setZValue(ItemZMap.value(ItemType::AttributeGrid, 0 ) );
    ui->graphicsView->scene()->addItem(item);

    auto w = new HistogramColorBarWidget;
    w->setOrientation(Qt::Horizontal);
    w->setColorTable(item->colorTable());
    w->setData( g->data(), g->size(), g->NULL_VALUE);
    w->setFixedSize( 200, 200);
    connect(item,SIGNAL(dataChanged(float*,size_t,float)),w,SLOT(setData(float*,size_t,float)));
    addItemLegendWidget(item,w,name);
}

void MapViewer2::addWellItem(QString name){

    auto info=m_project->getWellInfo(name);
    auto path=m_project->loadWellPath(name);
    if(!path){
        QMessageBox::critical(this, tr("Add Well"), QString("Loading well \"%1\" failed!").arg(name) );
        return;
    }
    auto item=new WellItem;

    item->setInfo(info);
    item->setPath(path);
    //item->setPos(info.x(), info.y());
    item->setData(ITEM_TYPE_INDEX, static_cast<int>(ItemType::Well) );
    item->setData(ITEM_NAME_INDEX, name);
    item->setZValue(ItemZMap.value(ItemType::Well, 0 ));

    item->setSize(m_defaultWellItem.size());
    item->setFont(m_defaultWellItem.font());
    //item->setPen(m_defaultWellItem.pen());
    //item->setBrush(m_defaultWellItem.brush());

    connect(this,SIGNAL(wellReferenceDepthChanged(double)),item,SLOT(setRefDepth(qreal)));
    ui->graphicsView->scene()->addItem(item);
}

void MapViewer2::on_actionSetup_Wells_triggered()
{
    auto scene = ui->graphicsView->scene();
    if( !scene ) return;

    OptionsDialog dlg;
    dlg.setWindowTitle(tr("Setup Well Display"));
    QStringList ltypes;
    ltypes<<toQString(WellItem::LabelType::NO_LABEL);
    ltypes<<toQString(WellItem::LabelType::UWI);
    ltypes<<toQString(WellItem::LabelType::WELL_NAME);
    dlg.addItem("Label-Type",QVariant::Type::String,ltypes);
    dlg.addItem("Label-Size",QVariant::Type::Int,0);
    dlg.addItem("Symbol-Size",QVariant::Type::Int,0);
    dlg.addItem("Symbol-Color",QVariant::Type::Color,0);
    dlg.addItem("Path-Color",QVariant::Type::Color,0);
    dlg.addItem("Z-Value",QVariant::Type::Int,0);

    dlg.setValue("Label-Type",toQString(m_defaultWellItem.labelType()));
    dlg.setValue("Label-Size",m_defaultWellItem.font().pointSize());
    dlg.setValue("Symbol-Size",m_defaultWellItem.size());
    dlg.setValue("Symbol-Color",m_defaultWellItem.symbolColor());
    dlg.setValue("Path-Color",m_defaultWellItem.pathColor());
    dlg.setValue("Z-Value",m_defaultWellItem.zValue());

    if(!dlg.exec()) return;

    m_defaultWellItem.setLabelType(toLabelType(dlg.getValue("Label-Type").toString()));
    QFont f=m_defaultWellItem.font();
    f.setPointSize(dlg.getValue("Label-Size").toInt());
    m_defaultWellItem.setFont(f);
    m_defaultWellItem.setSize( dlg.getValue("Symbol-Size").toInt() );
    m_defaultWellItem.setSymbolColor(dlg.getValue("Symbol-Color").value<QColor>());
    m_defaultWellItem.setPathColor(dlg.getValue("Path-Color").value<QColor>());
    m_defaultWellItem.setZValue(dlg.getValue("Z-Value").toInt());

    // now assign new options to all well items

    for( auto item : scene->items()){

        if( item->data(ITEM_TYPE_INDEX).toInt()==static_cast<int>(ItemType::Well)){

            auto wi=dynamic_cast<WellItem*>(item);
            if( !wi ) continue;  // better save than sorry

            wi->setLabelType(m_defaultWellItem.labelType());
            wi->setSize(m_defaultWellItem.size());
            wi->setFont(m_defaultWellItem.font());
            wi->setSymbolColor(m_defaultWellItem.symbolColor());
            wi->setPathColor(m_defaultWellItem.pathColor());
            wi->setZValue(m_defaultWellItem.zValue());
        }
    }
}

void MapViewer2::on_actionConfigure_Horizon_triggered()
{
    auto names=itemList(ItemType::HorizonGrid);
    QString iname;
    if(names.size()==0) return;
    else if(names.size()==1) iname=names[0];
    else{
        bool ok=false;
        iname=QInputDialog::getItem(this,"Configure Horizon","Select Horizon",names,0,false,&ok);
        if(!ok) return;
    }
    auto item=findItem(ItemType::HorizonGrid,iname);
    auto gitem=dynamic_cast<GridItem*>(item);
    if(gitem) configGridItem(gitem);
}

void MapViewer2::on_actionConfigure_Volume_triggered()
{
    auto names=itemList(ItemType::Volume);
    QString iname;
    if(names.size()==0) return;
    else if(names.size()==1) iname=names[0];
    else{
        bool ok=false;
        iname=QInputDialog::getItem(this,"Configure Volume","Select Volume",names,0,false,&ok);
        if(!ok) return;
    }
    auto item=findItem(ItemType::Volume,iname);
    auto vitem=dynamic_cast<VolumeItem*>(item);
    if(vitem) configVolumeItem(vitem);
}

void MapViewer2::on_actionConfigure_Area_triggered()
{
    auto names=itemList(ItemType::Area);
    QString iname;
    if(names.size()==0) return;
    else if(names.size()==1) iname=names[0];
    else{
        bool ok=false;
        iname=QInputDialog::getItem(this,"Configure Area","Select Area",names,0,false,&ok);
        if(!ok) return;
    }
    auto item=findItem(ItemType::Area,iname);
    auto aitem=dynamic_cast<AreaItem*>(item);
    if(aitem) configAreaItem(aitem);
}

void MapViewer2::configWellItem(WellItem * item){

    if( ! item) return;

    bool ok=false;
    QString text=QInputDialog::getText(nullptr, tr("Change Well Label"), tr("Enter Label Text:"),
                                QLineEdit::Normal, item->label(), &ok);
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
    dlg->setShowLabels(item->showLineLabels());
    dlg->setShowLabel(item->showLabel());
    dlg->setLabelText(item->label());
    dlg->setShowMesh(item->showMesh());
    dlg->setInlineIncrement(item->inlineIncrement());
    dlg->setCrosslineIncrement(item->crosslineIncrement());
    dlg->setZValue(item->zValue());
    dlg->setOpacity(item->opacity());
    connect(dlg, SIGNAL(showLabelsChanged(bool)), item, SLOT( setShowLineLabels(bool)));
    connect(dlg, SIGNAL(showLabelChanged(bool)), item, SLOT( setShowLabel(bool)));
    connect(dlg,SIGNAL(labelTextChanged(QString)), item, SLOT(setLabel(QString)));
    connect(dlg, SIGNAL(showMeshChanged(bool)), item, SLOT( setShowMesh(bool)));
    connect(dlg, SIGNAL(inlineIncrementChanged(int)), item, SLOT(setInlineIncrement(int)) );
    connect(dlg, SIGNAL(crosslineIncrementChanged(int)), item, SLOT(setCrosslineIncrement(int)) );
    connect(dlg,SIGNAL(zValueChanged(int)), item, SLOT(setZValueWrapper(int)));
    connect(dlg,SIGNAL(opacityChanged(double)),item,SLOT(setOpacityWrapper(double)));
    if( dlg->exec()==QDialog::Accepted){

    }
    else{

    }
    ui->graphicsView->scene()->update();

    delete dlg;
}


void MapViewer2::configVolumeItem(VolumeItem * item){

    if( !item ) return;
    if( !item->grid()) return;
    auto histogram=createHistogram( std::begin(*(item->volume())), std::end(*(item->volume())), item->volume()->NULL_VALUE, 100 );
    auto dlg=new GridItemConfigDialog;
    dlg->setWindowTitle(tr("Configure Volume Item"));
    dlg->setColorTable(item->colorTable());      // all updates of scling and colors done through colortable
    dlg->setHistogram(histogram);
    dlg->setShowLabels(item->showLineLabels());
    dlg->setShowLabel(item->showLabel());
    dlg->setLabelText(item->label());
    dlg->setShowMesh(item->showMesh());
    dlg->setInlineIncrement(item->inlineIncrement());
    dlg->setCrosslineIncrement(item->crosslineIncrement());
    dlg->setZValue(item->zValue());
    dlg->setOpacity(item->opacity());
    //auto bounds=item->volume()->bounds();
    connect(dlg, SIGNAL(showLabelsChanged(bool)), item, SLOT( setShowLineLabels(bool)));
    connect(dlg, SIGNAL(showLabelChanged(bool)), item, SLOT( setShowLabel(bool)));
    connect(dlg,SIGNAL(labelTextChanged(QString)), item, SLOT(setLabel(QString)));
    connect(dlg, SIGNAL(showMeshChanged(bool)), item, SLOT( setShowMesh(bool)));
    connect(dlg, SIGNAL(inlineIncrementChanged(int)), item, SLOT(setInlineIncrement(int)) );
    connect(dlg, SIGNAL(crosslineIncrementChanged(int)), item, SLOT(setCrosslineIncrement(int)) );
    connect(dlg,SIGNAL(zValueChanged(int)), item, SLOT(setZValueWrapper(int)));
    connect(dlg,SIGNAL(opacityChanged(double)),item,SLOT(setOpacityWrapper(double)));
    if( dlg->exec()==QDialog::Accepted){

    }
    else{

    }
    ui->graphicsView->scene()->update();
    delete dlg;
}


void MapViewer2::configAxis(GVRuler * gvruler){
    OptionsDialog dlg;
    dlg.setWindowTitle("Configure Axis");
    dlg.addItem("Automatic",QVariant::Type::Bool,0);
    dlg.addItem("Tick-Interval",QVariant::Double,0);
    dlg.addItem("Sub-Ticks",QVariant::Int,0);
    dlg.setValue("Automatic",gvruler->isAutoTickIncrement());
    dlg.setValue("Tick-Interval",gvruler->tickIncrement());
    dlg.setValue("Sub-Ticks",gvruler->subTickCount());

    if(dlg.exec()!=QDialog::Accepted) return;
    gvruler->setAutoTickIncrement(dlg.getValue("Automatic").toBool());
    gvruler->setTickIncrement(dlg.getValue("Tick-Interval").toDouble());
    gvruler->setSubTickCount(dlg.getValue("Sub-Ticks").toInt());
}

void MapViewer2::configAreaItem(AreaItem* item){
    OptionsDialog dlg;
    dlg.setWindowTitle("Configure Area");
    dlg.addItem("Z-Value",QVariant::Type::Int,0);
    dlg.addItem("Opacity",QVariant::Type::Int,0);
    dlg.addItem("Outline-Color",QVariant::Type::Color,0);
    dlg.addItem("Fill-Color",QVariant::Type::Color,0);
    dlg.addItem("Show Lines",QVariant::Type::Bool,0);
    dlg.setValue("Z-Value",item->zValue());
    dlg.setValue("Opacity",static_cast<int>(100*item->fillOpacity()));
    dlg.setValue("Outline-Color",item->outlineColor());
    dlg.setValue("Fill-Color",item->fillColor());
    dlg.setValue("Show Lines",item->isShowLines());

    if(dlg.exec()!=QDialog::Accepted) return;

    item->setShowLines(dlg.getValue("Show Lines").toBool());
    item->setZValue(dlg.getValue("Z-Value").toInt());
    item->setFillOpacity(0.01*dlg.getValue("Opacity").toInt());
    item->setOutlineColor(dlg.getValue("Outline-Color").value<QColor>());
    item->setFillColor(dlg.getValue("Fill-Color").value<QColor>());
    ui->graphicsView->scene()->update();
}

void MapViewer2::setupToolBarControls(){

    auto widget=new QWidget(this);
    m_cbDomain=new QComboBox;
    m_cbDomain->addItem(toQString(Domain::Depth));
    m_cbDomain->addItem(toQString(Domain::Time));
    m_sbSliceValue=new ReverseSpinBox;

    auto layout=new QHBoxLayout;
    layout->addWidget(m_cbDomain);
    layout->addWidget(m_sbSliceValue);
    widget->setLayout(layout);

    auto toolBar=addToolBar(tr("Controls Toolbar"));
    toolBar->addWidget(widget);

    m_sbSliceValue->setRange(-999999,999999);
    m_sbSliceValue->setValue(0);
    connect(m_sbSliceValue, SIGNAL(valueChanged(int)), this, SLOT(setSliceValue(int)));
    connect(m_cbDomain,SIGNAL(currentTextChanged(QString)),this, SLOT(setDomain(QString)));
}

void MapViewer2::addItemLegendWidget( QGraphicsItem* item, QWidget* w, QString title){
    w->setAttribute(Qt::WA_DeleteOnClose);
    Qt::WindowFlags flags;
    flags|=Qt::WindowTitleHint;
    flags|=Qt::WindowMaximizeButtonHint;
    flags|=Qt::WindowMinimizeButtonHint;
    auto sw=ui->legendArea->addSubWindow(w,flags);
    sw->setWindowTitle(title);
    sw->setFixedSize(200,200);
    sw->installEventFilter(this);
    w->show();
    m_legendWidgets.insert(item, sw);
//ui->legendArea->tileSubWindows();
}

void MapViewer2::removeItemLegendWidget( QGraphicsItem* item){
    if( ! m_legendWidgets.contains(item) ) return;
    auto w = m_legendWidgets.value(item);
    ui->legendArea->removeSubWindow(w);
    m_legendWidgets.remove(item);
//    ui->legendArea->tileSubWindows();
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


void MapViewer2::fillTree(){
    auto dom=domain();
    ui->treeWidget->clear();
    if(!m_project) return;
    ui->treeWidget->addItem("Horizons");
    for(auto name : m_project->gridList(GridType::Horizon)){
        ui->treeWidget->addItem("Horizons", name);
    }
    ui->treeWidget->addItem("Other Grids");
    for(auto name : m_project->gridList(GridType::Other)){
        ui->treeWidget->addItem("Other Grids", name);
    }
    ui->treeWidget->addItem("Attribute Grids");
    for(auto name : m_project->gridList(GridType::Attribute)){
        ui->treeWidget->addItem("Attribute Grids", name);
    }
    ui->treeWidget->addItem("Volumes");
    for(auto name : m_project->volumeList()){
        Domain vdomain;
        VolumeType vtype;
        try{
            m_project->getVolumeBounds(name, &vdomain, &vtype);
            if(dom==vdomain) ui->treeWidget->addItem("Volumes", name);
        }
        catch(...){}
    }
    ui->treeWidget->addItem("Wells",true);
    for(auto name : m_project->wellList()){
        if(m_project->existsWellPath(name)){	// only display wells that have a path
            ui->treeWidget->addItem("Wells", name);
        }
    }
}


