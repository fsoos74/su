#include "spectrumviewer.h"
#include "ui_spectrumviewer.h"

#include <frequencyspectra.h>   // function to generate spectrum from trace

#include <seismicdatasetreader.h>

#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include<QStandardItem>
#include<QStandardItemModel>
#include<QColorDialog>

SpectrumViewer::SpectrumViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::SpectrumViewer)
{
    ui->setupUi(this);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(runTableViewContextMenu(const QPoint&)));

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

    auto spectrum = generateSpectrum(def);
    auto color = COLORS[m_spectra.size() % COLORS.size()];
    m_spectra.insert( m_spectra.size(), Item{def, spectrum, color } );

    updateScene();
    updateTable();
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

void SpectrumViewer::receivePoint( SelectionPoint pt, int code ){

    // trigger adding of curve for given point
    if( code==PointCode::VIEWER_POINT_SELECTED){
        showSelector();
        selector->setInlineNumber(pt.iline);
        selector->setCrosslineNumber(pt.xline);
        selector->setWindowStartMS( static_cast<int>( std::round( 1000 * pt.time ) ) ); // convert to milliseconds
    }
}

void SpectrumViewer::receivePoints( QVector<SelectionPoint> points, int code){

    if( code!=CODE_SINGLE_POINTS) return;
    // add only first point from list if exists
    if( !points.empty()) receivePoint(points.front(), code);
}

void SpectrumViewer::updateScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    for( auto key : m_spectra.keys() ){

        auto const& item = m_spectra.value(key);
        const Spectrum& spectrum = item.spectrum;
        if( Spectrum::isNull(spectrum) ) continue;

        QPainterPath path;

        // this will scale to maximum amplitude, in first pass we ind it
        auto maxAmp = spectrum.amplitude(0);    // aplitude is always positive, signum is expressed by phase
        for( auto i=1; i<spectrum.size(); i++){
            auto const& amp = spectrum.amplitude(i);
            if( amp>maxAmp) maxAmp=amp;
        }
        if( !maxAmp) continue; // no valid values

        for( auto i=0; i<spectrum.size(); i++){
            auto f = spectrum.frequency(i);
            auto relAmp = spectrum.amplitude(i) / maxAmp;
            if( i>0 ){
                path.lineTo( f, relAmp );
            }
            else{       // first point
                path.moveTo( f, relAmp );
            }
        }

        QGraphicsPathItem* gitem=new QGraphicsPathItem(path);
        gitem->setPen( QPen( item.color, 0));
        scene->addItem( gitem );
    }

    QRectF bounds;
    // IMPORTANT: scene should never have an invalid scenerect, otherwise WEIRD problems with graphicsview!!!!!!
    if( scene->items().empty()){
        bounds=QRectF(0,0,1000,1000);
        scene->setSceneRect(bounds);
    }
    else{
        bounds=scene->itemsBoundingRect();
    }

    const qreal PADDING_FACTOR=1.;

    qreal xMargin=bounds.width()*PADDING_FACTOR;
    qreal yMargin=bounds.height()*PADDING_FACTOR;
    QRectF sceneRect=bounds.marginsAdded(QMarginsF(xMargin, yMargin, xMargin, yMargin));

    scene->setSceneRect(sceneRect);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->fitInView(sceneRect);
}

void SpectrumViewer::updateTable(){

    QStringList labels;
    labels<<"lookup*"<<"Color"<<"Dataset"<<"Inline"<<"Crossline"<<"Start[ms]"<<"length[ms]";

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
        model->setItem(row, column++, new QStandardItem(QString::number(info.windowStartMS )));
        model->setItem(row, column++, new QStandardItem(QString::number(info.windowLengthMS )));

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
        selector->setDatasetNames(m_project->seismicDatasetList());  // NEED to make this posstack only!!!


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

Spectrum SpectrumViewer::generateSpectrum( SpectrumDefinition def ){

    std::shared_ptr<SeismicDatasetReader> reader=m_project->openSeismicDataset(def.dataset);
    if( !reader){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve",
                              QString("Open dataset %1 failed!").arg(def.dataset) );
        return Spectrum();      // empty spectrum == NULL
    }

    auto trc = reader->readFirstTrace( "iline",QString::number(def.inlineNumber),
                                       "xline",QString::number(def.crosslineNumber) );
    if( !trc ){
        QMessageBox::critical(this, "Add Amplitude vs Offset Curve", "No data for this cdp!");
        return Spectrum();
    }

    double wstart=0.001 * def.windowStartMS;    // convert window start to seconds
    double wlen=0.001 * def.windowLengthMS;     // convert to seconds

    return computeSpectrum( *trc, wstart, wlen );
}
