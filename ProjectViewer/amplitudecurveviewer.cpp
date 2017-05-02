#include "amplitudecurveviewer.h"
#include "ui_amplitudecurveviewer.h"
#include <linearregression.h>
#include <amplitudecurves.h>
#include <gatherfilter.h>

#include<gather.h>
#include<trace.h>
#include<header.h>
#include<seismicdatasetreader.h>
#include<grid2d.h>
#include<QGraphicsScene>
#include<QGraphicsEllipseItem>
#include<QGraphicsLineItem>
#include<QMessageBox>
#include<QStandardItem>
#include<QStandardItemModel>
#include<QColorDialog>
#include<QInputDialog>
#include<iostream>

#include <dynamicmousemodeselector.h>

#include<datapointitem.h>


const int CURVE_INDEX_KEY=1;


PlotType string2PlotType( const QString& str){

   for( auto type : PlotTypesAndNames.keys()){
       if( str == PlotTypesAndNames.value(type) ){
           return type;
       }
   }

   throw std::runtime_error("Invalid plottype name in string2PlotType");
}

QString plotType2String( PlotType t ){

    for( auto type : PlotTypesAndNames.keys()){
        if( t == type ){
            return PlotTypesAndNames.value(type);
        }
    }

    throw std::runtime_error("Invalid type in plotType2String");
}


AmplitudeCurveViewer::AmplitudeCurveViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::AmplitudeCurveViewer)
{
    ui->setupUi(this);

    ui->graphicsView->topRuler()->setMinimumPixelIncrement(100);
    ui->graphicsView->topRuler()->setLabel("Offset");
    ui->graphicsView->leftRuler()->setLabel("Amplitude");
    ui->graphicsView->scale(1,-1); // flip vertical axxis, smallest at bottom, greated top

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runTableViewContextMenu(const QPoint&)));

    updateTable();

    connect( ui->graphicsView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );

    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    ui->graphicsView->setMouseTracking(true);
    ui->splitter->setStretchFactor(1,0);

    populateWindowMenu();

    setupMouseModes();

    loadSettings();
}

void AmplitudeCurveViewer::populateWindowMenu(){

    ui->menu_Window->addAction( ui->mouseToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->zoomToolBar->toggleViewAction());
    ui->menu_Window->addAction( ui->mainToolBar->toggleViewAction());
}

void AmplitudeCurveViewer::setupMouseModes(){
    DynamicMouseModeSelector* mm=new DynamicMouseModeSelector(this);
    connect( mm, SIGNAL(modeChanged(MouseMode)), ui->graphicsView, SLOT(setMouseMode(MouseMode)));
    mm->addMode(MouseMode::Explore);
    mm->addMode(MouseMode::Zoom);
    mm->addMode(MouseMode::Select);
    ui->mouseToolBar->addWidget( mm);
}

AmplitudeCurveViewer::~AmplitudeCurveViewer()
{
    delete ui;
}


void AmplitudeCurveViewer::receivePoint( SelectionPoint pt, int code ){

    // trigger adding of curve for given point
    if( code==PointCode::VIEWER_POINT_SELECTED){
        showSelector();
        selector->setInlineNumber(pt.iline);
        selector->setCrosslineNumber(pt.xline);
    }

}

void AmplitudeCurveViewer::receivePoints( QVector<SelectionPoint> points, int code){

    if( code!=CODE_SINGLE_POINTS) return;
    // add only first point from list if exists
    if( !points.empty()) receivePoint(points.front(), code);
}


void AmplitudeCurveViewer::setProject( AVOProject* project){
    m_project=project;
    if( selector){      // need to update this
        selector->setDatasetNames(m_project->seismicDatasetList());
        selector->setHorizonNames(m_project->gridList(GridType::Horizon));
    }
}


void AmplitudeCurveViewer::setShowRegressionLines(bool on){

    //std::cout<<std::boolalpha<<"show reglines: "<<on<<std::endl;
    if( on==m_showRegressionLines) return;

    m_showRegressionLines=on;

    updateScene();
}

void AmplitudeCurveViewer::setDatapointSize(int size){

    if( size==m_datapointSize) return;

    m_datapointSize=size;

    updateScene();
}


void AmplitudeCurveViewer::onMouseOver(QPointF scenePos){

    QString message=QString("Offset=%1, Amplitude=%2").arg(scenePos.x()).arg(scenePos.y());

    QPoint viewPos=ui->graphicsView->mapFromScene(scenePos);
    QGraphicsItem* item=ui->graphicsView->itemAt(viewPos);
    if( item ){
        DatapointItem* datapointItem=dynamic_cast<DatapointItem*>(item);
        if(datapointItem ){
            int curveIndex=datapointItem->data(CURVE_INDEX_KEY).toInt();
            const AmplitudeCurveDefinition& info=m_curveInfos.value(curveIndex);
            message.append(QString(", Inline=%1, Crossline=%2, Dataset=%3, Horizon=%4").arg(info.inlineNumber)
                           .arg(info.crosslineNumber).arg(info.dataset).arg(info.horizon));
        }
    }


    statusBar()->showMessage(message);
}

QVector<QPointF> AmplitudeCurveViewer::buildCurve( AmplitudeCurveDefinition def){

    ReductionMethod reductionMethod=string2ReductionMethod(def.reductionMethod);
    QVector<QPointF> curve;

    std::shared_ptr<SeismicDatasetReader> reader=m_project->openSeismicDataset(def.dataset);
    if( !reader){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve",
                              QString("Open dataset %1 failed!").arg(def.dataset) );
        return curve;
    }


    std::shared_ptr<seismic::Gather> gather=reader->readGather(
                                "iline",QString::number(def.inlineNumber),
                                        QString::number(def.inlineNumber+def.inlineSize-1),
                                "xline",QString::number(def.crosslineNumber),
                                        QString::number(def.crosslineNumber+def.crosslineSize-1) );

    if( gather->empty() ){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", "No data for this cdp!");
        return curve;
    }

    std::shared_ptr<Grid2D<float> > horizon=m_project->loadGrid( GridType::Horizon, def.horizon);
    if( !horizon ){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve",
                              QString("Loading horizon %1 failed!").arg(def.horizon) );
        return curve;
    }

    if( !horizon->bounds().isInside(def.inlineNumber, def.crosslineNumber)){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", QString("Horizon %1 does not contain inline %2 crossline %3!").
                              arg(def.horizon).arg(def.inlineNumber).arg(def.crosslineNumber) );
        return curve;
    }

    Grid2D<float>::value_type v=(*horizon)(def.inlineNumber, def.crosslineNumber);
    if( v == horizon->NULL_VALUE ){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", QString("Horizon %1 has NULL value at inline %2 crossline %3!").
                              arg(def.horizon).arg(def.inlineNumber).arg(def.crosslineNumber) );
        return curve;
    }

    double t=0.001 * v;    // horizon in millis

    std::unique_ptr<ReductionFunction> rf=reductionFunctionFactory(reductionMethod);

    //curve=buildAmplitudeOffsetCurve( *gather, t, def.maximumOffset, def.minimumAzimuth, def.maximumAzimuth, rf.get(), def.windowSize );

    // do trace selection
    GatherFilter filter(def.maximumOffset, def.minimumAzimuth, def.maximumAzimuth );
    gather=filter.filter(gather);
    if( gather->empty() ){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", "No traces in offset and azimuth range!");
        return curve;
    }


    curve=buildAmplitudeOffsetCurve( *gather, t, rf.get(), def.windowSize );


    return curve;
}

void AmplitudeCurveViewer::addCurve( AmplitudeCurveDefinition def){

    static const QVector<QColor> CurveColors{Qt::black, Qt::red, Qt::green, Qt::blue,
                Qt::yellow, Qt::magenta, Qt::cyan,
                Qt::gray};

    QVector<QPointF> curve=buildCurve( def);
    if( curve.isEmpty()) return;

    m_curves.insert(m_curveCounter, curve);

    QColor curveColor=CurveColors[m_curveCounter%CurveColors.size()];
    m_curveColors.insert( m_curveCounter, curveColor);


    m_curveInfos.insert(m_curveCounter, def);
    adaptCurveToPlottype(curve, def.depth);
    double quality;
    QPointF interceptAndGradient=linearRegression(curve, &quality);
    m_curveRegressions.insert( m_curveCounter, std::pair<QPointF,double>(interceptAndGradient, quality) );

    m_curveCounter++;

    //XXX
    updateScene();
    updateTable();
}

void AmplitudeCurveViewer::runTableViewContextMenu(const QPoint& pos){

    QPoint globalPos = ui->tableView->viewport()->mapToGlobal(pos);
    QModelIndex idx=ui->tableView->currentIndex();
    if( !idx.isValid()) return;

    int index=m_tableRowCurveIndexMap.at(idx.row());
    QMenu menu;
    menu.addAction("change color");
    menu.addSeparator();
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="change color" ){
        changeCurveColor(index);
    }
    else if( selectedAction->text()=="remove" ){
        removeCurve(index);
    }


}

void AmplitudeCurveViewer::changeCurveColor( int index ){

    QColor color=QColorDialog::getColor(m_curveColors.value(index), nullptr, "Choose curve color");
    if( color.isValid()){
        m_curveColors[index]=color;
        updateScene();
        updateTable();
    }
}


void AmplitudeCurveViewer::removeCurve( int index ){

    m_curves.remove(index);
    m_curveColors.remove(index);
    m_curveInfos.remove(index);
    m_curveRegressions.remove(index);
    updateScene();
    updateTable();
}

void AmplitudeCurveViewer::sceneSelectionChanged(){

    // can only select one item with double click
    if( ui->graphicsView->scene()->selectedItems().size()==1){

        QGraphicsItem* item=ui->graphicsView->scene()->selectedItems().front();
        int curveIndex=item->data( CURVE_INDEX_KEY).toInt();
        int iline=m_curveInfos[curveIndex].inlineNumber;
        int xline=m_curveInfos[curveIndex].crosslineNumber;
        double time=SelectionPoint::NO_TIME;
        sendPoint(  SelectionPoint(iline,xline, time), PointCode::VIEWER_POINT_SELECTED );
    }
}

void AmplitudeCurveViewer::adaptCurveToPlottype(QVector<QPointF>& curve, const double& depth){

    const double RADIANS_TO_DEGREES=57.29577951;

    if( m_plotType==PlotType::AmplitudeAngle || m_plotType==PlotType::AmplitudeSin2Angle){
        convertOffsetCurveToAngle(curve, depth);
    }

    if( m_plotType==PlotType::AmplitudeAngle){
        //convert to degrees
        for( QPointF& p : curve){
            double angle=p.x();
            p.setX( angle *RADIANS_TO_DEGREES);
        }
    }
    if( m_plotType==PlotType::AmplitudeSin2Angle){
        for( QPointF& p : curve){
            double angle=p.x();
            p.setX( std::pow(std::sin(angle),2));
        }
    }

}

void AmplitudeCurveViewer::updateRegressions(){

    m_curveRegressions.clear();

    for( int curveIndex : m_curves.keys()){

        // copy curve because need to modify it when using angles
        QVector<QPointF> curve=m_curves.value(curveIndex);

        adaptCurveToPlottype(curve, m_curveInfos.value(curveIndex).depth);

        double quality;
        QPointF interceptAndGradient=linearRegression(curve, &quality);
        m_curveRegressions.insert( curveIndex,
            std::pair<QPointF,double>(interceptAndGradient, quality)  );

    }

    updateTable();
}

template<typename RECT>
void dumpRect(RECT rect){
    std::cout<<"Rect: x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl;
}


void AmplitudeCurveViewer::updateScene(){

    const qreal ACTIVE_SIZE_FACTOR=2;           // datapoints grow by this factor when entered

    QRect datapointRect=QRect( -m_datapointSize/2, -m_datapointSize/2, m_datapointSize, m_datapointSize);

     QGraphicsScene* scene=new QGraphicsScene(this);

    for( int curveIndex : m_curves.keys()){

        // copy curve because need to modify it when using angles
        QVector<QPointF> curve=m_curves.value(curveIndex);

        // modify curve here f angle mode is selected
        adaptCurveToPlottype(curve, m_curveInfos.value(curveIndex).depth);

        QColor curveColor=m_curveColors.value( curveIndex);

        for( const QPointF& pt : curve){

            //QGraphicsEllipseItem* item=new QGraphicsEllipseItem;
            DatapointItem* item=new DatapointItem(m_datapointSize, ACTIVE_SIZE_FACTOR, curveColor, Qt::white );
            item->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
            //item->setRect(datapointRect);
            //item->setBrush(curveColor);

            // add user data inline and crossline
            item->setData( CURVE_INDEX_KEY, curveIndex);

            item->setPos( pt );

            scene->addItem(item);

        }
    }

    QRectF bounds;
    // IMPORTANT: scene should never have an invalid scenerect, otherwise WEIRD problems with graphicsview!!!!!!
    if( scene->items().empty()){
        bounds=QRectF(0,0,1000,1000);
        scene->setSceneRect(bounds);
    }
    else{
        bounds=scene->itemsBoundingRect();
        if(m_plotType==PlotType::AmplitudeAngle){
            bounds.setLeft(0);
            bounds.setRight(90);
        }
        else if(m_plotType==PlotType::AmplitudeSin2Angle){
            bounds.setLeft(0);
            bounds.setRight(1);
        }

    }


    if( m_showRegressionLines){

        qreal minX=0; // regression lines should run to 0 to allow intercept readout
        qreal maxX=bounds.right();
        for( int curveIndex : m_curves.keys()){

            const QVector<QPointF>& curve=m_curves.value(curveIndex);
            QColor curveColor=m_curveColors.value( curveIndex);

            QPointF interceptAndGradient=m_curveRegressions.value(curveIndex).first;
            double b=interceptAndGradient.x();
            double m=interceptAndGradient.y();
            qreal yleft=m*minX + b;
            qreal yright=m*maxX + b;
            QGraphicsLineItem* lineItem=new QGraphicsLineItem(minX, yleft, maxX, yright);
            lineItem->setPen(QPen(curveColor, 0));
            scene->addItem(lineItem);
        }
    }



    connect( scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()) );

    qreal xMargin=bounds.width()*X_PADDING_FACTOR;
    qreal yMargin=bounds.height()*Y_PADDING_FACTOR;
    QRectF sceneRect=bounds.marginsAdded(QMarginsF(xMargin, yMargin, xMargin, yMargin));

    scene->setSceneRect(sceneRect);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->fitInView(sceneRect);

    std::cout<<"graphicsview scenerect: ";dumpRect(ui->graphicsView->sceneRect());

    //ui->graphicsView->zoomFitWindow();

    std::cout<<"viewport geometry: ";dumpRect(ui->graphicsView->viewport()->geometry());

}

void AmplitudeCurveViewer::updateTable(){

    QStandardItemModel* model=new QStandardItemModel(m_curves.size(), 15, this);

    QStringList labels;
    labels<<"Color"<<"Inline"<<"Crossline"<<"Dataset"<<"Horizon"<<"Method"<<"Window Length"<<"Inlines"
         <<"Crosslines"<<"Max Offset"<<"Min Azimuth"<<"Max Azimuth"<<"Average Depth"
        <<"Intercept"<<"Gradient"<<"Quality";
    model->setHorizontalHeaderLabels(labels);

    m_tableRowCurveIndexMap.clear();

    int row=0;
    for( int index : m_curves.keys()){

        int column=0;

        const AmplitudeCurveDefinition& info=m_curveInfos.value(index);

        QColor curveColor=m_curveColors.value(index);
        QStandardItem* colorItem=new QStandardItem("");
        colorItem->setBackground(curveColor);
        model->setItem(row, column++, colorItem);

        model->setItem(row, column++, new QStandardItem(QString::number(info.inlineNumber)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.crosslineNumber)));
        model->setItem(row, column++, new QStandardItem(info.dataset));
        model->setItem(row, column++, new QStandardItem(info.horizon));
        model->setItem(row, column++, new QStandardItem(info.reductionMethod));
        model->setItem(row, column++, new QStandardItem(QString::number(info.windowSize)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.inlineSize)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.crosslineSize)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.maximumOffset)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.minimumAzimuth)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.maximumAzimuth)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.depth)));

        QPointF interceptAndGradient=m_curveRegressions.value(index).first;
        double quality=m_curveRegressions.value(index).second;
        model->setItem(row, column++, new QStandardItem(QString::number(interceptAndGradient.x())));
        model->setItem(row, column++, new QStandardItem(QString::number(interceptAndGradient.y())));
        model->setItem(row, column++, new QStandardItem(QString::number(quality)));

        m_tableRowCurveIndexMap.push_back(index);

        row++;
    }

   // ui->tableView->setColumnHidden(0, true); // don't show index, only internal use

    ui->tableView->setModel(model);
}


void AmplitudeCurveViewer::on_action_Add_Curve_triggered()
{
    showSelector();
}

void AmplitudeCurveViewer::showSelector(){

    if( !m_project) return;

    if( !selector ){

        selector=new AmplitudeCurveDataSelectionDialog(this);
        selector->setWindowTitle("Amplitude Curve");
        selector->setDatasetNames(m_project->seismicDatasetList());
        selector->setHorizonNames(m_project->gridList(GridType::Horizon));
        QStringList methods;
        for( auto method : ReductionMethodsAndNames.keys() ){
            methods.append( ReductionMethodsAndNames.value(method));
        }
        selector->setReductionMethods(methods);
        selector->setCurrentReductionMethod( ReductionMethodsAndNames.value(ReductionMethod::Mean));

        connect(selector, SIGNAL(curveDataSelected(AmplitudeCurveDefinition)),
                this, SLOT(addCurve(AmplitudeCurveDefinition)) );
    }

    selector->show();
}



void AmplitudeCurveViewer::on_actionConfigure_Items_triggered()
{
    if( !displayOptionsDialog){
        displayOptionsDialog=new CurveViewerDisplayOptionsDialog(this);
        displayOptionsDialog->setWindowTitle("Display Options");
        displayOptionsDialog->setDatapointSize(m_datapointSize);

        connect( displayOptionsDialog, SIGNAL(datapointSizeChanged(int)), this, SLOT(setDatapointSize(int)) );
        connect( displayOptionsDialog, SIGNAL(showRegressionLinesChanged(bool)), this, SLOT(setShowRegressionLines(bool)) );
    }

    displayOptionsDialog->show();
}

void AmplitudeCurveViewer::on_actionZoom_In_triggered()
{

    ui->graphicsView->zoomBy(ZOOM_IN_FACTOR);

}

void AmplitudeCurveViewer::on_actionZoom_Out_triggered()
{
    ui->graphicsView->zoomBy(ZOOM_OUT_FACTOR);

}

void AmplitudeCurveViewer::on_actionZoom_Fit_Window_triggered()
{

    ui->graphicsView->zoomFitWindow();
}

void AmplitudeCurveViewer::on_actionSelect_Plot_Type_triggered()
{
    bool ok=false;
    QString name=QInputDialog::getItem(0, "Select plot type", "Plot type:", PlotTypesAndNames.values(), 0, false, &ok);

    if( ok && !name.isEmpty()){
        setPlotTypeByName(name);
    }
}

void AmplitudeCurveViewer::setPlotTypeByName(QString name){

    PlotType t=string2PlotType(name);
    if( t==m_plotType) return;

    m_plotType=t;
    updateRegressions();

    ui->graphicsView->topRuler()->setLabel(PlotTypesAndXAxxisLabels.value(m_plotType));

    updateScene();
}

void AmplitudeCurveViewer::closeEvent(QCloseEvent *)
{
    saveSettings();
}

void AmplitudeCurveViewer::saveSettings(){

     QSettings settings(COMPANY, "AmplitudecurveViewer");

     settings.beginGroup("AmplitudecurveViewer");
       settings.setValue("size", size());
       //settings.setValue("position", pos() );
     settings.endGroup();

     settings.beginGroup("items");

        settings.setValue("datapoint-size", m_datapointSize);

     settings.endGroup();

     settings.sync();
}


void AmplitudeCurveViewer::loadSettings(){

    QSettings settings(COMPANY, "AmplitudecurveViewer");

    settings.beginGroup("AmplitudecurveViewer");
        resize(settings.value("size", QSize(600, 400)).toSize());
        //move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();


    settings.beginGroup("items");

          setDatapointSize( settings.value("datapoint-size", 7 ).toInt());

    settings.endGroup();
}
