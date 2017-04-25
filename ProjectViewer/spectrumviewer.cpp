#include "spectrumviewer.h"
#include "ui_spectrumviewer.h"

#include <frequencyspectra.h>   // function to generate spectrum from trace

#include <seismicdatasetreader.h>
#include <trace.h>
#include<datapointitem.h>

#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include<QStandardItem>
#include<QStandardItemModel>
#include<QColorDialog>
#include<QActionGroup>



SpectrumViewer::SpectrumViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::SpectrumViewer)
{
    ui->setupUi(this);

    // need to group mode options here because designer does not support this
    QActionGroup* modeGroup=new QActionGroup(this);
    modeGroup->setExclusive(true);
    modeGroup->addAction(ui->action_Amplitude);
    modeGroup->addAction(ui->action_Phase);
    connect( modeGroup, SIGNAL(triggered(QAction*)), this, SLOT(onModeGroupChanged()) );

    connect( ui->actionShow_Datapoints, SIGNAL(toggled(bool)), this, SLOT(updateScene() ) );

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runTableViewContextMenu(const QPoint&)));

    ui->graphicsView->setMouseTracking(true);
    connect( ui->graphicsView, SIGNAL(mouseOver(QPointF)), this, SLOT(onMouseOver(QPointF)) );

    ui->graphicsView->scale(1,-1);  // invert y-axis
    ui->graphicsView->leftRuler()->setLabel(tr("Relative Amplitude"));
    ui->graphicsView->topRuler()->setLabel(tr("Frequency [Hz]"));

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // read only

    updateScene();
    updateTable();
}

SpectrumViewer::~SpectrumViewer()
{
    delete ui;
}

void SpectrumViewer::setProject( AVOProject* p){

    if( p==m_project ) return;

    m_project=p;
}

void SpectrumViewer::addSpectrum(SpectrumDefinition def){

    static const std::vector<QColor> COLORS={
        Qt::red, Qt::green, Qt::blue, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };

    static int index = 0;

    auto spectrum = generateSpectrum(def);
    if(Spectrum::isNull(spectrum)){
        QMessageBox::warning(this, tr("Add Spectrum"), tr("Adding spectrum failed! Please check parameters."));
        return;
    }

    auto color = COLORS[ index % COLORS.size()];
    m_spectra.insert( index, Item{def, spectrum, color } );

    updateScene();
    updateTable();

     index++;
}

void SpectrumViewer::changeColor( int index ){

    auto it=m_spectra.find(index);  // need iterator because value does not give reference
    if( it==m_spectra.end()) return;  // key not in map

    QColor color=QColorDialog::getColor( it->color, nullptr, "Choose color");
    if( color.isValid()){
        it->color = color;
        updateScene();
        updateTable();
    }
}


void SpectrumViewer::removeSpectrum( int index ){

    m_spectra.remove(index);
    updateScene();
    updateTable();
}

void SpectrumViewer::setMode(Mode m){
    if( m_mode==m) return;
    m_mode=m;
    updateScene();
}

void SpectrumViewer::receivePoint( SelectionPoint pt, int code ){

    // trigger adding of curve for given point
    if( code==PointCode::VIEWER_POINT_SELECTED){
        showSelector();
        selector->setInlineNumber(pt.iline);
        selector->setCrosslineNumber(pt.xline);
        selector->setWindowCenterMS( static_cast<int>( std::round( 1000 * pt.time ) ) ); // convert to milliseconds
    }
}

void SpectrumViewer::receivePoints( QVector<SelectionPoint> points, int code){

    if( code!=CODE_SINGLE_POINTS) return;
    // add only first point from list if exists
    if( !points.empty()) receivePoint(points.front(), code);
}


void SpectrumViewer::onModeGroupChanged(){
    Mode m = (ui->action_Amplitude->isChecked() ) ? Mode::Amplitude : Mode::Phase;
    setMode(m);
}

QVector<QPointF> SpectrumViewer::buildAmplitudePath(const Spectrum& spectrum){

    QVector<QPointF> points;

    // this will scale to maximum amplitude, in first pass we find it
    auto maxAmp = spectrum.amplitude(0);    // aplitude is always positive, signum is expressed by phase
    for( auto i=1; i<spectrum.size(); i++){
        auto const& amp = spectrum.amplitude(i);
        if( amp>maxAmp) maxAmp=amp;
    }
    if( !maxAmp) return points;

    for( auto i=0; i<spectrum.size(); i++){
        auto f = spectrum.frequency(i);
        auto relAmp = spectrum.amplitude(i) / maxAmp;
        points.push_back( QPointF( f, relAmp) );
    }

    return points;
}

QVector<QPointF> SpectrumViewer::buildPhasePath(const Spectrum& spectrum){

    QVector<QPointF> points;

    for( auto i=0; i<spectrum.size(); i++){
        auto f = spectrum.frequency(i);
        auto phase = spectrum.phase(i);
        points.push_back(QPointF( f, phase ) );
    }

    return points;
}

void SpectrumViewer::updateScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    const int DATAPOINT_SIZE_PIX = 4;

    QRectF bb;      // compute own bounding box without datapoints

    for( auto key : m_spectra.keys() ){

        auto const& item = m_spectra.value(key);
        const Spectrum& spectrum = item.spectrum;
        if( Spectrum::isNull(spectrum) ) continue;

        // find points
        QVector<QPointF> points;
        switch( m_mode ){
        case Mode::Amplitude: points=buildAmplitudePath(spectrum) ; break;
        case Mode::Phase: points=buildPhasePath(spectrum); break;
        }
        if( points.size()<2) continue;

        // add polyline through points
        QPainterPath path;
        path.moveTo(points[0]);
        for( int i=1; i<points.size(); i++) path.lineTo(points[i]);
        QGraphicsPathItem* gitem=new QGraphicsPathItem(path);
        gitem->setPen( QPen( item.color, 0));
        scene->addItem( gitem );

        bb=bb.united( gitem->boundingRect() );

        //add datapoints if desired
        if( ui->actionShow_Datapoints->isChecked()){
            for( auto const& p : points){
                DatapointItem* ditem=new DatapointItem( DATAPOINT_SIZE_PIX, 1.5*DATAPOINT_SIZE_PIX, item.color, Qt::white );
                ditem->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
                //item->setFlag(QGraphicsItem::ItemIsSelectable, true);

                // add user data inline and crossline
                //item->setData( CURVE_INDEX_KEY, curveIndex);

                ditem->setPos( p );

                scene->addItem(ditem);
            }
        }
    }

    QRectF bounds;
    // IMPORTANT: scene should never have an invalid scenerect, otherwise WEIRD problems with graphicsview!!!!!!
    if( scene->items().empty()){
        bounds=QRectF(0,0,1000,1);
        scene->setSceneRect(bounds);
    }
    else{
        //bounds=scene->itemsBoundingRect();  // don't use this because need to eclude datapoints
       bounds=bb;
    }

    scene->setSceneRect(bounds);
    ui->graphicsView->setScene(scene);

    ui->graphicsView->fitInView(bounds);
    ui->graphicsView->update(); // this a hack to update the rulers, need to find a better way to do it
}

void SpectrumViewer::updateTable(){

    QStringList labels;
    labels<<"lookup*"<<"Color"<<"Dataset"<<"Inline"<<"Crossline"<<"Start[ms]"<<"Samples";

    QStandardItemModel* model=new QStandardItemModel(m_spectra.size(), labels.size(), this);
    model->setHorizontalHeaderLabels(labels);

    int row=0;
    for( auto index : m_spectra.keys()){

        auto const& item = m_spectra.value(index);
        const SpectrumDefinition& info=item.info;

        int column=0;

         model->setItem(row, column++, new QStandardItem(QString::number(index)));  // hidden index

        auto const& color=item.color;
        QStandardItem* colorItem=new QStandardItem("");
        colorItem->setBackground(color);
        model->setItem(row, column++, colorItem);

        model->setItem(row, column++, new QStandardItem(info.dataset));
        model->setItem(row, column++, new QStandardItem(QString::number(info.inlineNumber)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.crosslineNumber)));
        model->setItem(row, column++, new QStandardItem(QString::number(info.windowCenterMS )));
        model->setItem(row, column++, new QStandardItem(QString::number(info.windowSamples )));

        row++;
    }

    ui->tableView->setModel(model);
    ui->tableView->hideColumn( 0 );  // don't show index, only internal use
}

void SpectrumViewer::showSelector(){

    if( !m_project) return;

    if( !selector ){

        selector=new SpectrumDataSelectionDialog(this);
        selector->setWindowTitle("Frequency Spectrum");

        selector->setDatasetNames(m_project->seismicDatasetList(SeismicDatasetInfo::Mode::Poststack));

        connect(selector, SIGNAL(dataSelected(SpectrumDefinition)),
                this, SLOT(addSpectrum(SpectrumDefinition)) );
    }

    selector->show();
}

void SpectrumViewer::on_action_Add_Spectrum_triggered()
{
    showSelector();
}

void SpectrumViewer::runTableViewContextMenu(const QPoint& pos){

    QPoint globalPos = ui->tableView->viewport()->mapToGlobal(pos);
    QModelIndex idx=ui->tableView->currentIndex();
    if( !idx.isValid()) return;

    QStandardItemModel* model=dynamic_cast<QStandardItemModel*>(ui->tableView->model());
    if( !model ) return;

    int row=idx.row();
    const int col=0;    // index lookup is first column
    QStandardItem* item = model->item( row, col );

    int index = item->text().toInt();

    QMenu menu;
    menu.addAction("change color");
    menu.addSeparator();
    menu.addAction("remove");

    QAction* selectedAction = menu.exec(globalPos);
    if (!selectedAction) return;

    if( selectedAction->text()=="change color" ){
        changeColor(index);
    }
    else if( selectedAction->text()=="remove" ){
        removeSpectrum(index);
    }


}

void SpectrumViewer::onMouseOver(QPointF scenePos){

    QString ylabel;
    switch(m_mode){
    case Mode::Amplitude: ylabel=tr("Relative Amplitude"); break;
    case Mode::Phase: ylabel=tr("Phase"); break;
    }

    QString message=QString("Frequency=%1, %2=%3").arg(scenePos.x()).arg(ylabel).arg(scenePos.y());
/*
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

*/
    statusBar()->showMessage(message);
}

Spectrum SpectrumViewer::generateSpectrum( SpectrumDefinition def ){

    std::shared_ptr<SeismicDatasetReader> reader=m_project->openSeismicDataset(def.dataset);
    if( !reader){
        QMessageBox::critical(this, "Add Spectrum",
                              QString("Open dataset %1 failed!").arg(def.dataset) );
        return Spectrum();      // empty spectrum == NULL
    }

    auto trc = reader->readFirstTrace( "iline",QString::number(def.inlineNumber),
                                       "xline",QString::number(def.crosslineNumber) );
    if( !trc ){
        QMessageBox::critical(this, "Add  Spectrum", "No data for this cdp!");
        return Spectrum();
    }

    auto window_center = trc->time_to_index(0.001 * def.windowCenterMS);

    return computeSpectrum( *trc, window_center, def.windowSamples );
}
