#include "xyzimportdialog.h"
#include "ui_xyzimportdialog.h"


#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include<iostream>

#include<QProgressDialog>
#include<QDoubleValidator>
#include<QStringList>

#include<QDebug>

XYZImportDialog::XYZImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XYZImportDialog)
{
    ui->setupUi(this);
    auto validator=new QDoubleValidator(this);
    ui->leX0->setValidator(validator);
    ui->leY0->setValidator(validator);
    ui->leZ0->setValidator(validator);
    ui->leSX->setValidator(validator);
    ui->leSY->setValidator(validator);
    ui->leSZ->setValidator(validator);
}

XYZImportDialog::~XYZImportDialog()
{
    delete ui;
}

int XYZImportDialog::xColumn(){
    return ui->sbX->value();
}

int XYZImportDialog::yColumn(){
    return ui->sbY->value();
}

int XYZImportDialog::zColumn(){
    return ui->sbZ->value();
}

double XYZImportDialog::x0(){
    return ui->leX0->text().toDouble();
}

double XYZImportDialog::y0(){
    return ui->leY0->text().toDouble();
}

double XYZImportDialog::z0(){
    return ui->leZ0->text().toDouble();
}

double XYZImportDialog::sx(){
    return ui->leSX->text().toDouble();
}

double XYZImportDialog::sy(){
    return ui->leSY->text().toDouble();
}

double XYZImportDialog::sz(){
    return ui->leSZ->text().toDouble();
}

int XYZImportDialog::skipLines(){
    return ui->sbSkip->value();
}

QString XYZImportDialog::nullValue(){
    return ui->leNull->text();
}

QString XYZImportDialog::filename(){
    return ui->lePath->text();
}

QVector<QVector3D> XYZImportDialog::loadData(){
    load();
    return m_data;
}

void XYZImportDialog::setSkipLines( int n ){

    ui->sbSkip->setValue(n);
}

void XYZImportDialog::setXColumn( int col ){
    ui->sbX->setValue(col);
}

void XYZImportDialog::setYColumn( int col ){
    ui->sbY->setValue(col);
}

void XYZImportDialog::setZColumn( int col ){
    ui->sbZ->setValue(col);
}

void XYZImportDialog::setX0(double d){
    ui->leX0->setText(QString::number(d));
}

void XYZImportDialog::setY0(double d){
    ui->leY0->setText(QString::number(d));
}

void XYZImportDialog::setZ0(double d){
    ui->leZ0->setText(QString::number(d));
}

void XYZImportDialog::setSX(double d){
    ui->leSX->setText(QString::number(d));
}

void XYZImportDialog::setSY(double d){
    ui->leSY->setText(QString::number(d));
}

void XYZImportDialog::setSZ(double d){
    ui->leSZ->setText(QString::number(d));
}

void  XYZImportDialog::setNullValue( const QString& s ){
    ui->leNull->setText(s);
}

void XYZImportDialog::setFilename(const QString & s){
    ui->lePath->setText(s);
    updatePreview(s);
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

    updateParamsFromControls();

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

        if( !processLine(line, x, y, z)) continue;

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

    updateParamsFromControls();

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

        if( n<=m_skipLines){
            continue;
        }

        double x,y,z;

        if( !processLine(line, x, y, z)) continue;

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

bool XYZImportDialog::processLine( const QString& line, double& x, double& y, double& z){

     QRegExp sep("(\\s+)");

     QStringList col=line.split(sep, QString::SkipEmptyParts);

     if( col.size()<=m_lastUsedColumn){
        return false;
     }

     if( m_nullValue==col[m_zColumn] ){
         return false;
     }

     bool ok;
     x=m_x0+m_sx*col[m_xColumn].toDouble(&ok);
     if( !ok ) return false;
     y=m_y0+m_sy*col[m_yColumn].toDouble(&ok);
     if( !ok ) return false;
     z=m_z0+m_sz*col[m_zColumn].toDouble(&ok);
     if( !ok ) return false;

     return true;
}

 void XYZImportDialog::updateParamsFromControls(){
     m_filename=filename();
     m_nullValue=nullValue();
     m_skipLines=skipLines();
     m_xColumn=xColumn()-1;     // make 0 based
     m_yColumn=yColumn()-1;     // make 0 based
     m_zColumn=zColumn()-1;     // make 0 based
     m_x0=x0();
     m_y0=y0();
     m_z0=z0();
     m_sx=sx();
     m_sy=sy();
     m_sz=sz();

     // update last used column
     int l=m_xColumn;
     if( l<m_yColumn) l=m_yColumn;
     if( l<m_zColumn) l=m_zColumn;
     m_lastUsedColumn=l;
 }
