#include "gridviewer.h"
#include "ui_gridviewer.h"

#include<QMessageBox>
#include<QFileDialog>
#include<QInputDialog>
#include<QFile>
#include<QDir>
#include<QColorDialog>
#include<iostream>
#include<cmath>

#include<gridimportdialog.h>
#include<colortabledialog.h>
#include<isolinedialog.h>
#include<orientationdialog.h>
#include<xgrreader.h>
#include<xgrwriter.h>
#include<isolinecomputer.h>
#include"griddisplayoptionsdialog.h"



GridViewer::GridViewer(QWidget *parent) :
    BaseViewer(parent),
    ui(new Ui::GridViewer)
{
    ui->setupUi(this);

    setAttribute( Qt::WA_DeleteOnClose);

    m_gridView = ui->gridView;//new GridView(this);
    colorBar=ui->colorBar;
    m_gridView->setBackgroundRole(QPalette::Dark);
    //setCentralWidget(gridView);

    connect( m_gridView->label(), SIGNAL( mouseOver(int,int)), this, SLOT( onGridViewMouseOver(int,int)) );

    setDefaultColorTable();

    setWindowTitle(tr("Grid Viewer"));


    //resize(800, 600);

    qRegisterMetaTypeStreamOperators<AxxisOrientation>("AxxisOrientation");
    qRegisterMetaTypeStreamOperators<AxxisDirection>("AxxisDirection");


    connect( m_gridView, SIGNAL(pointSelected(QPoint)), this, SLOT(onViewPointSelected(QPoint)) );

    connect( m_gridView, SIGNAL(polylineSelected(QVector<QPoint>)), this, SLOT(onViewPolylineSelected(QVector<QPoint>)) );
    connect( ui->action_Receive_CDPs, SIGNAL(toggled(bool)), this, SLOT(setReceptionEnabled(bool)) );
    connect( ui->action_Dispatch_CDPs, SIGNAL(toggled(bool)), this, SLOT(setBroadcastEnabled(bool)) );

    loadSettings();


}

GridViewer::~GridViewer()
{
    delete ui;
}

bool GridViewer::orientate(const ProjectGeometry& geom){

    if( !isValid(geom)) return false;

    // p0: origin, p1: far end 1st inline, p2: far end first xline

    qreal dx10=(geom.coordinates(1).x()-geom.coordinates(0).x());
    qreal dy10=(geom.coordinates(1).y()-geom.coordinates(0).y());

    qreal dx20=(geom.coordinates(2).x()-geom.coordinates(0).x());
    qreal dy20=(geom.coordinates(2).y()-geom.coordinates(0).y());

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

void GridViewer::receivePoint( SelectionPoint point, int code ){

    QVector<SelectionPoint> v{point};

    gridView()->setHighlightedCDPs(v);
}

void GridViewer::receivePoints( QVector<SelectionPoint> points, int code){

    if( code==CODE_SINGLE_POINTS){

        gridView()->setHighlightedCDPs(points);
    }
}

void GridViewer::setProject(std::shared_ptr<AVOProject> p){

    if( p==m_project ) return;

    m_project=p;

    update();
}

void GridViewer::setGrid( std::shared_ptr<Grid2D<float> > grid){

    if( grid==m_grid) return;

    m_grid=grid;
    gridView()->setGrid(grid);
    gridView()->setColorMapping( valueRange(*m_grid));

    gridView()->zoomFit();
}

void GridViewer::setDefaultColorTable(){

    QVector<QRgb> baseColors=ColorTable::defaultColors();


    gridView()->colorTable()->setColors(baseColors);
    colorBar->setColorTable(gridView()->colorTable());

}

void GridViewer::onGridViewMouseOver(int i, int j){

    if( !m_grid) return;

    if( i<m_grid->bounds().i1() ){
        qWarning("i<ilmin");
        return;
    }

    if( i>m_grid->bounds().i2() ){
        qWarning("i>ilmax");
        return;
    }

    if( j<m_grid->bounds().j1() ){
        qWarning("j<xlmin");
        return;
    }

    if( j>m_grid->bounds().j2() ){
        qWarning("j>xlmax");
        return;
    }

    QString msg=QString::asprintf("inline=%d  crossline=%d  ", i, j);


    QTransform IlXlToXY;
    QTransform XYToIlXl;
    if( m_project && m_project->geometry().computeTransforms( XYToIlXl, IlXlToXY)){


            QPointF p=IlXlToXY.map( QPoint(i, j));
            qreal x=p.x();
            qreal y=p.y();

            msg+=QString::asprintf("x=%.1lf  y=%.1lf  ", x, y);
    }

    double val=(*m_grid)( i, j);

    if( val!=Grid2D<float>::NULL_VALUE){
        msg+=QString::asprintf("value=%.2lf  ", val);
    }
    else{
        msg+=tr("value=NULL");
    }

    statusBar()->showMessage(msg);
}

void GridViewer::on_zoomInAct_triggered()
{
    gridView()->zoomBy( 1.25);
}

void GridViewer::on_zoomOutAct_triggered()
{
    gridView()->zoomBy(0.8);
}

void GridViewer::on_zoomNormalAct_triggered()
{
    gridView()->zoomFit();
}

void GridViewer::on_displayRangeAct_triggered()
{

    if(!displayRangeDialog){

        displayRangeDialog=new DisplayRangeDialog(this);

        displayRangeDialog->setPower(gridView()->colorTable()->power());
        displayRangeDialog->setRange(gridView()->colorTable()->range());
        connect( displayRangeDialog, SIGNAL(rangeChanged(std::pair<double,double>)),
                 gridView()->colorTable(), SLOT(setRange(std::pair<double,double>)) );
        connect( displayRangeDialog, SIGNAL(powerChanged(double)),
                 gridView()->colorTable(), SLOT( setPower(double)) );
    }

    displayRangeDialog->show();

}

void GridViewer::on_EditColorTableAct_triggered()
{
    QVector<QRgb> oldColors=gridView()->colorTable()->colors();

    ColorTableDialog* colorTableDialog=new ColorTableDialog( oldColors);

    Q_ASSERT(colorTableDialog);

    connect( colorTableDialog, SIGNAL(colorsChanged(QVector<QRgb>)), gridView(), SLOT(setColors(QVector<QRgb>)));

    if( colorTableDialog->exec()==QDialog::Accepted ){
        gridView()->setColors( colorTableDialog->colors());
    }else{
        gridView()->setColors( oldColors );
    }

    delete colorTableDialog;
}

/*
void GridViewer::on_setBackgroundColorAct_triggered()
{
    const QColor color = QColorDialog::getColor( gridView->nullColor(), this, "Select Background Color");


    if (color.isValid()) {

      gridView->setNullColor(color);

    }
}
*/

void GridViewer::on_actionSetup_Contours_triggered()
{
    if(!isoLineDialog){
        std::pair<double, double> range=gridView()->colorTable()->range();
        isoLineDialog=new IsoLineDialog(this);
        isoLineDialog->setFirst(range.first);
        isoLineDialog->setLast(range.second);
        isoLineDialog->setLineColor(gridView()->viewLabel()->isoLineColor());
        isoLineDialog->setLineWidth(gridView()->viewLabel()->isoLineWidth());
        isoLineDialog->setColorBarSteps(colorBar->ticks());
        connect( isoLineDialog, SIGNAL( lineColorChanged(QColor)), gridView()->label(), SLOT(setIsoLineColor(QColor)));
        connect( isoLineDialog, SIGNAL( lineWidthChanged(int)), gridView()->label(), SLOT( setIsoLineWidth(int)));
        connect( isoLineDialog, SIGNAL( contoursChanged(const QVector<double>&)), gridView(), SLOT( setIsoLineValues( const QVector<double>&) ));
        connect( colorBar, SIGNAL(ticksChanged( const QVector<double>&)), isoLineDialog, SLOT(setColorBarSteps(QVector<double>)) );
    }

    isoLineDialog->show();

}

void GridViewer::on_actionAxxis_Orientation_triggered()
{

    OrientationDialog orientationDialog(this);
    orientationDialog.setInlineOrientation(gridView()->inlineOrientation());
    orientationDialog.setInlineDirection(gridView()->inlineDirection());
    orientationDialog.setCrosslineDirection(gridView()->crosslineDirection());
    orientationDialog.setWindowTitle(tr("Configure Viewer Axis Orientation"));


    if( orientationDialog.exec()==QDialog::Accepted){

        gridView()->setInlineOrientation(orientationDialog.inlineOrientation());
        gridView()->setInlineDirection(orientationDialog.inlineDirection() );
        gridView()->setCrosslineDirection(orientationDialog.crosslineDirection() );
    }
}

void GridViewer::on_action_Grid_Display_triggered()
{

    if( !gridDisplayOptionsDialog ){

        gridDisplayOptionsDialog=new GridDisplayOptionsDialog(this);
        gridDisplayOptionsDialog->setTransformationMode( gridView()->viewLabel()->transformationMode());
        gridDisplayOptionsDialog->setBackgroundColor( gridView()->nullColor());
        gridDisplayOptionsDialog->setHighlightedCDPSize( gridView()->viewLabel()->highlightCDPSize());
        gridDisplayOptionsDialog->setHighlightedCDPColor( gridView()->viewLabel()->highlightCDPColor());
        connect( gridDisplayOptionsDialog, SIGNAL(transformationModeChanged(Qt::TransformationMode)),
                 gridView()->viewLabel(), SLOT( setTransformationMode(Qt::TransformationMode)) );
        connect( gridDisplayOptionsDialog, SIGNAL( backgroundColorChanged(QColor)),
                 gridView(), SLOT(setNullColor(QColor)) );
        connect( gridDisplayOptionsDialog, SIGNAL(highlightedCDPSizeChanged(int)),
                 gridView()->viewLabel(), SLOT(setHighlightCDPSize(int)) );
        connect( gridDisplayOptionsDialog, SIGNAL(highlightedCDPColorChanged(QColor)),
                 gridView()->viewLabel(),SLOT(setHighlightCDPColor(QColor)) );
    }

    gridDisplayOptionsDialog->show();
}


void GridViewer::on_actionAspect_Ratio_triggered()
{
    if( !aspectRatioDialog ){

        aspectRatioDialog=new AspectRatioDialog(this);
        aspectRatioDialog->setWindowTitle(tr("Define Aspect Ratio"));

        aspectRatioDialog->setDefaultRatio(gridView()->computeILXLBasedAspectRatio()); // need to update this on inline orientation change!!!
        aspectRatioDialog->setRatio(gridView()->aspectRatio());
        aspectRatioDialog->setFixed(gridView()->isFixedAspectRatio());

        connect( aspectRatioDialog, SIGNAL(ratioChanged(double)), gridView(), SLOT(setAspectRatio(qreal)) );
        connect( aspectRatioDialog, SIGNAL(fixedChanged(bool)), gridView(), SLOT(setFixedAspectRatio(bool)) );

        connect( gridView(), SIGNAL(fixedAspectRatioChanged(bool)), aspectRatioDialog, SLOT(setFixed(bool)) );
        connect( gridView(), SIGNAL(aspectRatioChanged(qreal)), aspectRatioDialog, SLOT(setRatio(double)) );
    }

    aspectRatioDialog->show();
}

void GridViewer::on_actionConfigure_Scales_triggered()
{
    if( !scaleDialog ){

        scaleDialog=new ScaleDialog(this);
        scaleDialog->setWindowTitle(tr("Configure Scales"));

        scaleDialog->setHorizontalIncrement( gridView()->horizontalRuler()->tickIncrement() );
        scaleDialog->setHorizontalFixed(gridView()->horizontalRuler()->isFixedTickIncrement() );
        connect( scaleDialog, SIGNAL(horizontalFixedChanged(bool)), gridView()->horizontalRuler(), SLOT(setFixedTickIncrement(bool)) );
        connect( scaleDialog, SIGNAL(horizontalIncrementChanged(int)), gridView()->horizontalRuler(), SLOT(setTickIncrement(int)) );
        connect( gridView()->horizontalRuler(), SIGNAL(fixedTickIncrementChanged(bool)), scaleDialog, SLOT(setHorizontalFixed(bool)) );
        connect( gridView()->horizontalRuler(), SIGNAL(tickIncrementChanged(int)), scaleDialog, SLOT(setHorizontalIncrement(int)) );

        scaleDialog->setVerticalIncrement( gridView()->verticalRuler()->tickIncrement() );
        scaleDialog->setVerticalFixed(gridView()->verticalRuler()->isFixedTickIncrement() );
        connect( scaleDialog, SIGNAL(verticalFixedChanged(bool)), gridView()->verticalRuler(), SLOT(setFixedTickIncrement(bool)) );
        connect( scaleDialog, SIGNAL(verticalIncrementChanged(int)), gridView()->verticalRuler(), SLOT(setTickIncrement(int)) );
        connect( gridView()->verticalRuler(), SIGNAL(fixedTickIncrementChanged(bool)), scaleDialog, SLOT(setVerticalFixed(bool)) );
        connect( gridView()->verticalRuler(), SIGNAL(tickIncrementChanged(int)), scaleDialog, SLOT(setVerticalIncrement(int)) );
    }

    scaleDialog->show();
}


void GridViewer::on_actionConfigure_Colorbar_triggered()
{
    if( !colorBarConfigurationDialog){
        colorBarConfigurationDialog=new ColorBarConfigurationDialog(this);
        colorBarConfigurationDialog->setWindowTitle(tr("Configure Colorbar"));
        colorBarConfigurationDialog->setScaleRange(ui->colorBar->range());

        colorBarConfigurationDialog->setScaleSteps(ui->colorBar->steps());
        connect(colorBarConfigurationDialog, SIGNAL(scaleRangeChanged(std::pair<double,double>)),
                                                    ui->colorBar, SLOT(setRange(std::pair<double,double>)));
        connect(colorBarConfigurationDialog, SIGNAL(scaleStepsChanged(int)),
                ui->colorBar, SLOT(setSteps(int)));

        connect( gridView()->colorTable(), SIGNAL(rangeChanged(std::pair<double,double>)),
                 colorBarConfigurationDialog, SLOT(setScaleRange(std::pair<double,double>) ) );    // update dialog if range is changed
    }

    colorBarConfigurationDialog->show();
}


// need this to forward point from view to dispatcher
void GridViewer::onViewPointSelected( QPoint point){

    sendPoint(SelectionPoint( point.x(), point.y() ), VIEWER_CURRENT_CDP);
}

void GridViewer::onViewPolylineSelected( QVector<QPoint> polyline){

    QVector<SelectionPoint> sline;
    sline.reserve(polyline.size());
    for( QPoint p:polyline ){
        sline.append(SelectionPoint(p.y(), p.x() ));
    }
    sendPoints( sline, CODE_POLYLINE);
}

void GridViewer::closeEvent(QCloseEvent *)
{
    saveSettings();
}

QMap<Qt::TransformationMode, QString> TransformationModeMap{
    {Qt::FastTransformation, "Fast"},
    {Qt::SmoothTransformation, "Smooth"}
};

QString transformationModeToString(Qt::TransformationMode mode){

   return TransformationModeMap.value(mode);
}

Qt::TransformationMode stringToTransformationMode( QString str){

    return TransformationModeMap.key(str);
}

void GridViewer::saveSettings(){

     QSettings settings(COMPANY, "GridViewer");

     settings.beginGroup("GridViewer");
       settings.setValue("size", size());
       settings.setValue("position", pos() );
     settings.endGroup();



     settings.beginGroup("gridview");

/* axis orientation is now stored per project in project-files
       settings.setValue("inline orientation", QVariant::fromValue(gridView->inlineOrientation()) );

       settings.setValue("inline direction", QVariant::fromValue(gridView->inlineDirection()) );

       settings.setValue("crossline direction", QVariant::fromValue(gridView->crosslineDirection()) );
*/
       settings.setValue("inline axxis label", gridView()->inlineAxxisLabel());

       settings.setValue("crossline axxis label", gridView()->crosslineAxxisLabel());

       settings.setValue("null color", QVariant::fromValue(gridView()->nullColor()));

       settings.setValue("isoline-width", gridView()->viewLabel()->isoLineWidth());

       settings.setValue("isoline-color", QVariant::fromValue(gridView()->viewLabel()->isoLineColor()));

       settings.setValue("highlight-cdp-size", gridView()->viewLabel()->highlightCDPSize());

       settings.setValue("highlight-cdp-color", QVariant::fromValue(gridView()->viewLabel()->highlightCDPColor()));

       settings.setValue("image-transformation-mode", transformationModeToString( gridView()->viewLabel()->transformationMode()) );

     settings.endGroup();



     settings.sync();
}


void GridViewer::loadSettings(){

    QSettings settings(COMPANY, "GridViewer");

    settings.beginGroup("GridViewer");
        resize(settings.value("size", QSize(600, 400)).toSize());
        move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

    QString s;

    settings.beginGroup("gridview");

          QVariant value;

/* axis orientation is now stored per project in project-files
          gridView->setInlineOrientation(
            settings.value("inline orientation", QVariant::fromValue(AxxisOrientation::Horizontal)).value<AxxisOrientation>());

          gridView->setInlineDirection(
            settings.value("inline direction", QVariant::fromValue(AxxisDirection::Ascending)).value<AxxisDirection>());

          gridView->setCrosslineDirection(
            settings.value("crossline direction", QVariant::fromValue(AxxisDirection::Ascending)).value<AxxisDirection>());
*/
          gridView()->setInlineAxxisLabel( settings.value("inline axxis label", "Inline Number").toString());

          gridView()->setCrosslineAxxisLabel( settings.value("crossline axxis label", "Crossline Number").toString());

          gridView()->setNullColor(
                      settings.value("null color", QVariant::fromValue(Qt::gray)).value<QColor>());

          gridView()->viewLabel()->setIsoLineWidth( settings.value("isoline-width", 1 ).toInt());

          gridView()->viewLabel()->setIsoLineColor(
                      settings.value("isoline-color",QVariant::fromValue(Qt::darkGray)).value<QColor>());

          gridView()->viewLabel()->setHighlightCDPSize(settings.value("highlight-cdp-size", 6).toInt());

          gridView()->viewLabel()->setHighlightCDPColor(
                      settings.value("highlight-cdp-color",QVariant::fromValue(Qt::red)).value<QColor>());

          QString str=settings.value("image-transformation-mode", "SMOOTH").toString();
          gridView()->viewLabel()->setTransformationMode(stringToTransformationMode(str));

    settings.endGroup();
}



void GridViewer::on_actionDisplay_Histogram_triggered()
{

    if( ! m_grid ) return;

    QVector<double> data;
    for( auto it=m_grid->values().cbegin(); it!=m_grid->values().cend(); ++it){
        if( *it==m_grid->NULL_VALUE) continue;
        data.push_back(*it);
     }

    HistogramDialog* viewer=new HistogramDialog(this);      // need to make this a parent in order to allow qt to delete this when this is deleted
                                                            // this is important because otherwise the colortable will be deleted before this! CRASH!!!
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1").arg(windowTitle() ) );
    viewer->setColorTable(gridView()->colorTable());        // the colortable must have same parent as viewer, maybe used shared_ptr!!!
    viewer->show();

}





