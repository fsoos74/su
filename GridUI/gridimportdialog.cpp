#include "gridimportdialog.h"
#include "ui_gridimportdialog.h"

#include <grid2d.h>

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include<iostream>

#include<QProgressDialog>
#include<QStringList>

#include<QDebug>

GridImportDialog::GridImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridImportDialog)
{
    ui->setupUi(this);
    connectToUI();
}

GridImportDialog::~GridImportDialog()
{
    delete ui;
}


void GridImportDialog::setSkipLines( int n ){

    if( n==m_skipLines) return;

    if( n<0 ){
        qWarning( "Skiplines cannot be less than zero!");
        return;
    }

    m_skipLines=n;

    emit( skipLinesChanged(m_skipLines));
}

// col is assumed 1-based
void GridImportDialog::setInlineColumn( int col ){

    col--;

    if( col == m_inlineColumn ) return;

    if( col < 0 ){
        qWarning( "Inline column cannot be less than zero!");
        return;
    }

    m_inlineColumn=col;

    updateLastUsedColumn();

    emit( inlineColumnChanged(col+1)); // make 1-based
}

// col is assumed 1-based
void GridImportDialog::setCrosslineColumn( int col ){

    col--;

    if( col == m_crosslineColumn ) return;

    if( col<0 ){
        qWarning( "Crossline column cannot be less than zero!");
        return;
    }

    m_crosslineColumn=col;

    updateLastUsedColumn();

    emit( crosslineColumnChanged(col+1));  // make 1-based
}

// col is assumed 1-based
void GridImportDialog::setValueColumn( int col ){

    col--;

    if( col == m_valueColumn ) return;

    if( col<0 ){
        qWarning( "Value column cannot be less than zero!");
        return;
    }

    m_valueColumn=col;

    updateLastUsedColumn();

    emit( valueColumnChanged(col+1));        // make 1-based
}

void  GridImportDialog::setNullValue( const QString& s ){

    if( s==m_nullValue ) return;

    m_nullValue = s;

    emit( nullValueChanged( s ) );
}

void GridImportDialog::setFilename(const QString & s){

    if( s==m_filename) return;

    m_filename=s;

    emit( filenameChanged(s) );
}

void GridImportDialog::setInlineRange(int min, int max){


    if( min==m_minInline && max==m_maxInline ) return;

    if( min>max ){
        qWarning("Maximum inline cannot be less than minimum!");
        return;
    }

    m_minInline=min;
    m_maxInline=max;

    ui->leMinIline->setText( QString::number(min));
    ui->leMaxIline->setText( QString::number(max));

    emit( inlineRangeChanged(min,max));

    setValidBounds(false);
}

void GridImportDialog::setCrosslineRange(int min, int max){

    if( min==m_minCrossline && max==m_maxCrossline ) return;

    if( min>max ){
        qWarning("Maximum crossline cannot be less than minimum!");
        return;
    }

    m_minCrossline=min;
    m_maxCrossline=max;

    ui->leMinXline->setText( QString::number(min));
    ui->leMaxXline->setText( QString::number(max));

    emit( crosslineRangeChanged(min,max));

    setValidBounds(false);
}

void GridImportDialog::setValueRange(double min, double max){

    if( min==m_minValue && max==m_maxValue ) return;

    if( min>max ){
        qWarning("Maximum value cannot be less than minimum!");
        return;
    }

    m_minValue=min;
    m_maxValue=max;

    ui->leMinValue->setText( QString::number(min));
    ui->leMaxValue->setText( QString::number(max));

    emit( valueRangeChanged(min,max));
}

void GridImportDialog::setValidBounds(bool b){

    if( b==m_validBounds) return;

    m_validBounds=b;

    emit validBoundsChanged(b);
}

void GridImportDialog::setHorizon(std::shared_ptr<Grid2D<float> > h){

    if( h==m_horizon) return;

    m_horizon=h;

    emit( horizonChanged(h) );

    setInlineRange( m_horizon->bounds().j1(), m_horizon->bounds().j2());
    setCrosslineRange( m_horizon->bounds().i1(), m_horizon->bounds().i2());

    // XXX add value range update
}

void GridImportDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Open Horizon file");

    if( fn.isNull()) return;        // cancelled

    setFilename(fn);
}


void GridImportDialog::updatePreview(const QString& filename)
{

    ui->teContent->clear();

    QFile file( filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    int i=0;
    while(!in.atEnd() && i<PreviewLines){
        QString line=in.readLine();
        ui->teContent->append(line);
        i++;
    }



   ui->teContent -> moveCursor (QTextCursor::Start) ;
   ui->teContent -> ensureCursorVisible() ;



}

void GridImportDialog::scan(){

    QFile file( m_filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(m_filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);



    int min_iline=std::numeric_limits<int>::max();
    int max_iline=-std::numeric_limits<int>::max();
    int min_xline=std::numeric_limits<int>::max();
    int max_xline=-std::numeric_limits<int>::max();
    double min_value=std::numeric_limits<double>::max();
    double max_value=-std::numeric_limits<double>::max();


    int max=file.bytesAvailable();
    QProgressDialog* pdlg=new QProgressDialog("Scanning Horizon Geometry...", "Cancel", 0, max, this);
    //pdlg->show();
    int i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();
        int counter=max-file.bytesAvailable();

        if( i<=m_skipLines){
            continue;
        }

        int iline;
        int xline;
        double value;

        if( !splitLine(line, iline, xline, value)) continue;


        if(iline<min_iline ) min_iline=iline;
        if( iline>max_iline ) max_iline=iline;

        if(xline<min_xline ) min_xline=xline;
        if( xline>max_xline ) max_xline=xline;

        if(value<min_value ) min_value=value;
        if( value>max_value ) max_value=value;


        pdlg->setValue(counter);

        qApp->processEvents();

        if( pdlg->wasCanceled()){
            pdlg->setValue(max);
            return; // do not update min/max values
        }

    }

    pdlg->setValue(max);

    setInlineRange( min_iline, max_iline);
    setCrosslineRange( min_xline, max_xline);
    setValueRange(min_value, max_value);

    setValidBounds(true);
}


void GridImportDialog::loadHorizon(){

    if( !validBounds()){
        scan();
    }

    std::shared_ptr<Grid2D<float> > g(new Grid2D<float>(Grid2DBounds(m_minInline, m_minCrossline, m_maxInline, m_maxCrossline) ) );


    QFile file( m_filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(m_filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    int max=file.bytesAvailable();
    QProgressDialog* pdlg=new QProgressDialog("Loading Horizon...", "Cancel", 0, max, this);
    int i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();
        int counter=max-file.bytesAvailable();

        if( i<=m_skipLines){
            continue;
        }

        int iline;
        int xline;
        double value;

        if( !splitLine(line, iline, xline, value)) continue;


        (*g)(iline, xline)=value;

        pdlg->setValue(counter);

        qApp->processEvents();

        if( pdlg->wasCanceled()){
            pdlg->setValue(max);
            return; // do not update min/max values
        }

    }

    pdlg->setValue(max);

    setHorizon(g);
}

void GridImportDialog::accept(){

    loadHorizon();
    QDialog::accept();
}

void GridImportDialog::on_pbScan_clicked()
{
    scan();
}

bool GridImportDialog::splitLine( const QString& line, int& iline, int& xline, double& value){


     QRegExp sep("(\\s+)");

     QStringList col=line.split(sep, QString::SkipEmptyParts);

//     std::cout<<col.size()<<" "<<iline_col<<" "<<xline_col<<" "<<value_col<<" "<<last_used_col<<std::endl;
//     return false;


     if( col.size()<=m_lastUsedColumn){
        return false;
     }

     if( m_nullValue==col[m_valueColumn] ){
         return false;
     }

     bool ok;
     iline=col[m_inlineColumn].toInt(&ok);
     if( !ok ) return false;

     xline=col[m_crosslineColumn].toInt(&ok);
     if( !ok ) return false;

     value=col[m_valueColumn].toDouble(&ok);
     if( !ok ) return false;

     return true;
}

 void GridImportDialog::connectToUI(){

     connect( this, SIGNAL( skipLinesChanged(int)), ui->sbSkip, SLOT(setValue(int)));
     connect( ui->sbSkip, SIGNAL(valueChanged(int)), this, SLOT( setSkipLines(int)));

     connect( this, SIGNAL( inlineColumnChanged(int)), ui->sbIline, SLOT(setValue(int)));
     connect( ui->sbIline, SIGNAL(valueChanged(int)), this, SLOT( setInlineColumn(int)));

     connect( this, SIGNAL( crosslineColumnChanged(int)), ui->sbXline, SLOT(setValue(int)));
     connect( ui->sbXline, SIGNAL(valueChanged(int)), this, SLOT( setCrosslineColumn(int)));

     connect( this, SIGNAL( valueColumnChanged(int)), ui->sbValue, SLOT( setValue(int)));
     connect( ui->sbValue, SIGNAL( valueChanged(int)), this, SLOT( setValueColumn(int)));

     connect( this, SIGNAL(nullValueChanged(QString)), ui->leNull, SLOT( setText(QString)));
     connect( ui->leNull, SIGNAL(textChanged(QString)), this, SLOT(setNullValue(QString)));

     connect( this, SIGNAL( filenameChanged(QString)), this, SLOT( updatePreview(QString)));
     connect( this, SIGNAL( filenameChanged(QString)), ui->lePath, SLOT( setText(QString)));
 }

 void GridImportDialog::updateLastUsedColumn(){

     int l=m_inlineColumn;
     if( l<m_crosslineColumn) l=m_crosslineColumn;
     if( l<m_valueColumn) l=m_valueColumn;
     m_lastUsedColumn=l;
 }
