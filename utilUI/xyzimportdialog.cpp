#include "xyzimportdialog.h"
#include "ui_xyzimportdialog.h"


#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include<iostream>

#include<QProgressDialog>
#include<QStringList>

#include<QDebug>

XYZImportDialog::XYZImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XYZImportDialog)
{
    ui->setupUi(this);
    connectToUI();
}

XYZImportDialog::~XYZImportDialog()
{
    delete ui;
}


QVector<QVector3D> XYZImportDialog::loadData(){
    load();
    return m_data;
}

void XYZImportDialog::setSkipLines( int n ){

    if( n==m_skipLines) return;

    if( n<0 ){
        qWarning( "Skiplines cannot be less than zero!");
        return;
    }

    m_skipLines=n;

    emit( skipLinesChanged(m_skipLines));
}

// col is assumed 1-based
void XYZImportDialog::setXColumn( int col ){

    col--;

    if( col == m_xColumn ) return;

    if( col < 0 ){
        qWarning( "X column cannot be less than zero!");
        return;
    }

    m_xColumn=col;

    updateLastUsedColumn();

    emit( xColumnChanged(col+1)); // make 1-based
}

// col is assumed 1-based
void XYZImportDialog::setYColumn( int col ){

    col--;

    if( col == m_yColumn ) return;

    if( col<0 ){
        qWarning( "Y column cannot be less than zero!");
        return;
    }

    m_yColumn=col;

    updateLastUsedColumn();

    emit( yColumnChanged(col+1));  // make 1-based
}

// col is assumed 1-based
void XYZImportDialog::setZColumn( int col ){

    col--;

    if( col == m_zColumn ) return;

    if( col<0 ){
        qWarning( "Z column cannot be less than zero!");
        return;
    }

    m_zColumn=col;

    updateLastUsedColumn();

    emit( zColumnChanged(col+1));        // make 1-based
}

void  XYZImportDialog::setNullValue( const QString& s ){

    if( s==m_nullValue ) return;

    m_nullValue = s;

    emit( nullValueChanged( s ) );
}

void XYZImportDialog::setFilename(const QString & s){

    if( s==m_filename) return;

    m_filename=s;

    emit( filenameChanged(s) );
}

void XYZImportDialog::setXRange(std::pair<double, double> r){

    if( r == m_xRange ) return;

    if( r.first>r.second ){
        qWarning("Maximum X cannot be less than minimum!");
        return;
    }

    m_xRange = r;

    ui->leMinX->setText( QString::number(r.first));
    ui->leMaxX->setText( QString::number(r.second));

    emit( xRangeChanged(r));
}

void XYZImportDialog::setYRange(std::pair<double, double> r){

    if( r == m_yRange ) return;

    if( r.first>r.second ){
        qWarning("Maximum Y cannot be less than minimum!");
        return;
    }

    m_yRange = r;

    ui->leMinY->setText( QString::number(r.first));
    ui->leMaxY->setText( QString::number(r.second));

    emit( yRangeChanged(r));
}

void XYZImportDialog::setZRange(std::pair<double, double> r){

    if( r == m_zRange ) return;

    if( r.first>r.second ){
        qWarning("Maximum Z cannot be less than minimum!");
        return;
    }

    m_zRange = r;

    ui->leMinZ->setText( QString::number(r.first));
    ui->leMaxZ->setText( QString::number(r.second));

    emit( zRangeChanged(r));
}


void XYZImportDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Open XYZ file", QFileInfo(ui->lePath->text()).absoluteDir().absolutePath()  );

    if( fn.isNull()) return;        // cancelled

    setFilename(fn);
}


void XYZImportDialog::updatePreview(const QString& filename)
{

    ui->teContent->clear();

    QFile file( filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, this->windowTitle(),
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

void XYZImportDialog::scan(){

    QFile file( m_filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, this->windowTitle(),
                             tr("Cannot read file %1:\n%2.")
                             .arg(m_filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    double minX=std::numeric_limits<double>::max();
    double maxX=std::numeric_limits<double>::lowest();
    double minY=std::numeric_limits<double>::max();
    double maxY=std::numeric_limits<double>::lowest();
    double minZ=std::numeric_limits<double>::max();
    double maxZ=std::numeric_limits<double>::lowest();

    int max=file.bytesAvailable();
    QProgressDialog* pdlg=new QProgressDialog("Scanning XYZ file...", "Cancel", 0, max, this);
    
    int n=0;

    while(!in.atEnd()){

        n++;

        QString line=in.readLine();
        int counter=max-file.bytesAvailable();

        if( n<=m_skipLines){
            continue;
        }

        double x,y,z;

        if( !splitLine(line, x, y, z)) continue;

        if( x<minX) minX=x;
        if( x>maxX) maxX=x;
        if( y<minY) minY=y;
        if( y>maxY) maxY=y;
        if( z<minZ) minZ=z;
        if( z>maxZ) maxZ=z;

        pdlg->setValue(counter);

        qApp->processEvents();

        if( pdlg->wasCanceled()){
            pdlg->setValue(max);
            return; // do not update min/max values
        }

    }

    pdlg->setValue(max);

    setXRange( std::make_pair(minX, maxX) );
    setYRange( std::make_pair(minY, maxY) );
    setZRange( std::make_pair(minZ, maxZ) );
}


void XYZImportDialog::load(){

    m_data.clear();

    QFile file( m_filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, windowTitle(),
                             tr("Cannot read file %1:\n%2.")
                             .arg(m_filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    int max=file.bytesAvailable();
    QProgressDialog* pdlg=new QProgressDialog("Loading XYZ data...", "Cancel", 0, max, this);
    
    int n=0;

    while(!in.atEnd()){

        n++;

        QString line=in.readLine();
        int counter=max-file.bytesAvailable();

        if( n<=m_skipLines){
            continue;
        }

        double x,y,z;

        if( !splitLine(line, x, y, z)) continue;

        m_data.push_back( QVector3D(x,y,z) );

        qApp->processEvents();

        if( pdlg->wasCanceled()){
            pdlg->setValue(max);
            return; // do not update min/max values
        }

    }

    pdlg->setValue(max);
}

void XYZImportDialog::accept(){

    load();
    QDialog::accept();
}

void XYZImportDialog::on_pbScan_clicked()
{
    scan();
}

bool XYZImportDialog::splitLine( const QString& line, double& x, double& y, double& z){

     QRegExp sep("(\\s+)");

     QStringList col=line.split(sep, QString::SkipEmptyParts);

     if( col.size()<=m_lastUsedColumn){
        return false;
     }

     if( m_nullValue==col[m_zColumn] ){
         return false;
     }

     bool ok;
     x=col[m_xColumn].toDouble(&ok);
     if( !ok ) return false;
     y=col[m_yColumn].toDouble(&ok);
     if( !ok ) return false;
     z=col[m_zColumn].toDouble(&ok);
     if( !ok ) return false;

     return true;
}

 void XYZImportDialog::connectToUI(){

     connect( this, SIGNAL( skipLinesChanged(int)), ui->sbSkip, SLOT(setValue(int)));
     connect( ui->sbSkip, SIGNAL(valueChanged(int)), this, SLOT( setSkipLines(int)));

     connect( this, SIGNAL( xColumnChanged(int)), ui->sbX, SLOT(setValue(int)));
     connect( ui->sbX, SIGNAL(valueChanged(int)), this, SLOT( setXColumn(int)));

     connect( this, SIGNAL( yColumnChanged(int)), ui->sbY, SLOT(setValue(int)));
     connect( ui->sbY, SIGNAL(valueChanged(int)), this, SLOT( setYColumn(int)));

     connect( this, SIGNAL( zColumnChanged(int)), ui->sbZ, SLOT( setValue(int)));
     connect( ui->sbZ, SIGNAL( valueChanged(int)), this, SLOT( setZColumn(int)));

     connect( this, SIGNAL(nullValueChanged(QString)), ui->leNull, SLOT( setText(QString)));
     connect( ui->leNull, SIGNAL(textChanged(QString)), this, SLOT(setNullValue(QString)));

     connect( this, SIGNAL( filenameChanged(QString)), this, SLOT( updatePreview(QString)));
     connect( this, SIGNAL( filenameChanged(QString)), ui->lePath, SLOT( setText(QString)));
 }

 void XYZImportDialog::updateLastUsedColumn(){

     int l=m_xColumn;
     if( l<m_yColumn) l=m_yColumn;
     if( l<m_zColumn) l=m_zColumn;
     m_lastUsedColumn=l;
 }
