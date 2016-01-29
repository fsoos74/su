#include "amplitudecurveviewer.h"
#include "ui_amplitudecurveviewer.h"
#include <linearregression.h>
#include <amplitudecurves.h>

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

    loadSettings();
}

AmplitudeCurveViewer::~AmplitudeCurveViewer()
{
    delete ui;
}


void AmplitudeCurveViewer::receivePoint( QPoint pt ){

    // trigger adding of curve for given point
    showSelector();
    selector->setInlineNumber(pt.x());
    selector->setCrosslineNumber(pt.y());


}

void AmplitudeCurveViewer::receivePoints( QVector<QPoint> points, int code){

    if( code!=CODE_SINGLE_POINTS) return;
    // add only first point from list if exists
    if( !points.empty()) receivePoint(points.front());
}


void AmplitudeCurveViewer::setProject( std::shared_ptr<AVOProject> project){
    m_project=project;
    if( selector){      // need to update this
        selector->setDatasetNames(m_project->seismicDatasetList());
        selector->setHorizonNames(m_project->gridList(GridType::Horizon));
    }
}


void AmplitudeCurveViewer::setShowRegressionLines(bool on){

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
            const CurveInfo& info=m_curveInfos.value(curveIndex);
            message.append(QString(", Inline=%1, Crossline=%2, Dataset=%3, Horizon=%4").arg(info.inlineNumber)
                           .arg(info.crosslineNumber).arg(info.dataset).arg(info.horizon));
        }
    }


    statusBar()->showMessage(message);
}

QVector<QPointF> AmplitudeCurveViewer::buildCurve( const QString& datasetName, const QString& horizonName,
                                                         int inlineNumber, int crosslineNumber,
                                                         int inlineSize, int crosslineSize, double maximumOffset,
                                                         ReductionMethod reductionMethod, int windowSize){

    QVector<QPointF> curve;

    std::shared_ptr<SeismicDatasetReader> reader=m_project->openSeismicDataset(datasetName);
    if( !reader){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", QString("Open dataset %1 failed!").arg(datasetName) );
        return curve;
    }


    std::shared_ptr<seismic::Gather> gather=reader->readGather("iline",QString::number(inlineNumber),QString::number(inlineNumber+inlineSize-1),
                                "xline",QString::number(crosslineNumber),QString::number(crosslineNumber+crosslineSize-1) );

    if( gather->empty() ){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", "No data for this cdp!");
        return curve;
    }

    std::shared_ptr<Grid2D<double> > horizon=m_project->loadGrid( GridType::Horizon, horizonName);
    if( !horizon ){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", QString("Loading horizon %1 failed!").arg(horizonName) );
        return curve;
    }

    if( !horizon->bounds().isInside(inlineNumber, crosslineNumber)){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", QString("Horizon %1 does not contain inline %2 crossline %3!").
                              arg(horizonName).arg(inlineNumber).arg(crosslineNumber) );
        return curve;
    }

    Grid2D<double>::value_type v=(*horizon)(inlineNumber, crosslineNumber);
    if( v == horizon->NULL_VALUE ){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", QString("Horizon %1 has NULL value at inline %2 crossline %3!").
                              arg(horizonName).arg(inlineNumber).arg(crosslineNumber) );
        return curve;
    }

    double t=0.001 * v;    // horizon in millis

    std::unique_ptr<ReductionFunction> rf=reductionFunctionFactory(reductionMethod);

    curve=buildAmplitudeOffsetCurve( *gather, t, maximumOffset, rf.get(), windowSize );

    return curve;
}

void AmplitudeCurveViewer::addCurve( QString datasetName, QString horizonName, int inlineNumber, int crosslineNumber,
                                            int inlineSize, int crosslineSize, double maximumOffset, double depth,
                                     QString reductionMethodName, int windowSize){

    static const QVector<QColor> CurveColors{Qt::black, Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::magenta, Qt::cyan, Qt::gray};

    ReductionMethod reductionMethod=string2ReductionMethod(reductionMethodName);
    QVector<QPointF> curve=buildCurve( datasetName, horizonName, inlineNumber, crosslineNumber,
                                       inlineSize, crosslineSize, maximumOffset, reductionMethod, windowSize);
    if( curve.isEmpty()) return;

    m_curves.insert(m_curveCounter, curve);

    QColor curveColor=CurveColors[m_curveCounter%CurveColors.size()];
    m_curveColors.insert( m_curveCounter, curveColor);

    CurveInfo info;
    info.dataset=datasetName;
    info.horizon=horizonName;
    info.inlineNumber=inlineNumber;
    info.crosslineNumber=crosslineNumber;
    info.inlineSize=inlineSize;
    info.crosslineSize=crosslineSize;
    info.maximumOffset=maximumOffset;
    info.depth=depth;
    info.reductionMethod=reductionMethodName;
    info.windowSize=windowSize;
    m_curveInfos.insert(m_curveCounter, info);

    adaptCurveToPlottype(curve, depth);
    m_curveRegressions.insert( m_curveCounter, linearRegression(curve) );

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

        sendPoint(  QPoint(iline,xline) );
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

        m_curveRegressions.insert( curveIndex, linearRegression(curve) );

    }

    updateTable();
}

template<typename RECT>
void dumpRect(RECT rect){
    std::cout<<"Rect: x="<<rect.x()<<" y="<<rect.y()<<" w="<<rect.width()<<" h="<<rect.height()<<std::endl;
}


void AmplitudeCurveViewer::updateScene(){

    const qreal ACTIVE_SIZE_FACTOR=2;           // datapoints grow by this factor when entered

    QGraphicsScene* scene=new QGraphicsScene(this);

    QRect datapointRect=QRect( -m_datapointSize/2, -m_datapointSize/2, m_datapointSize, m_datapointSize);

    qreal minX=std::numeric_limits<qreal>::max();
    qreal maxX=std::numeric_limits<qreal>::lowest();
    qreal minY=std::numeric_limits<qreal>::max();
    qreal maxY=std::numeric_limits<qreal>::lowest();

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

            if(pt.x()<minX) minX=pt.x();
            if(pt.x()>maxX) maxX=pt.x();
            if(pt.y()<minY) minY=pt.y();
            if(pt.y()>maxY) maxY=pt.y();
        }
    }


    if( minX>maxX) std::swap(minX,maxX);
    if( minY>maxY) std::swap(minY,maxY);

    if( m_showRegressionLines){

        minX=0; // regression lines should run to 0 to allow intercept readout

        for( int curveIndex : m_curves.keys()){

            const QVector<QPointF>& curve=m_curves.value(curveIndex);
            QColor curveColor=m_curveColors.value( curveIndex);

            QPointF interceptAndGradient=m_curveRegressions.value(curveIndex);
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

    ui->graphicsView->setScene(scene);
    QRectF rect(minX,minY,maxX-minX,maxY-minY);//=scene->itemsBoundingRect();
    qreal xMargin=rect.width()*X_PADDING_FACTOR;
    qreal yMargin=rect.height()*Y_PADDING_FACTOR;
    QRectF sceneRect=rect.marginsAdded(QMarginsF(xMargin, yMargin, xMargin, yMargin));

    scene->setSceneRect(sceneRect);
    ui->graphicsView->fitInView(sceneRect);

    //ui->graphicsView->zoomFitWindow();

}

void AmplitudeCurveViewer::updateTable(){

    QStandardItemModel* model=new QStandardItemModel(m_curves.size(), 12, this);

    QStringList labels;
    labels<<"Color"<<"Inline"<<"Crossline"<<"Dataset"<<"Horizon"<<"Method"<<"Window Length"<<"Inlines"
         <<"Crosslines"<<"Max Offset"<<"Average Depth"<<"Intercept"<<"Gradient";
    model->setHorizontalHeaderLabels(labels);

    m_tableRowCurveIndexMap.clear();

    int row=0;
    for( int index : m_curves.keys()){

        int column=0;

        const CurveInfo& info=m_curveInfos.value(index);

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
        model->setItem(row, column++, new QStandardItem(QString::number(info.depth)));

        QPointF interceptAndGradient=m_curveRegressions.value(index);
        model->setItem(row, column++, new QStandardItem(QString::number(interceptAndGradient.x())));
        model->setItem(row, column++, new QStandardItem(QString::number(interceptAndGradient.y())));

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

        // NEED TO MAKE A STRUCT FOR THIS !!!XXX
        connect(selector, SIGNAL(curveDataSelected(QString,QString,int,int,int,int,double, double, QString,int)),
                this, SLOT(addCurve(QString,QString,int,int,int,int,double, double, QString,int)) );
    }

    selector->show();
}



void AmplitudeCurveViewer::on_actionConfigure_Items_triggered()
{
    if( !displayOptionsDialog){
        displayOptionsDialog=new CurveViewerDisplayOptionsDialog(this);
        displayOptionsDialog->setWindowTitle("Display Options");
        displayOptionsDialog->setDatapointSize(m_datapointSize);
        displayOptionsDialog->setShowRegressionLines(m_showRegressionLines);

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
