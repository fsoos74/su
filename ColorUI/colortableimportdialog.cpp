#include "colortableimportdialog.h"
#include "ui_colortableimportdialog.h"

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include<QDoubleValidator>
#include<iostream>

#include<QProgressDialog>
#include<QStringList>

#include<QDebug>

ColortableImportDialog::ColortableImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColortableImportDialog)
{
    ui->setupUi(this);

    QDoubleValidator* validator=new QDoubleValidator(this);
    ui->cbScaler->setValidator(validator);

    connectToUI();
}

ColortableImportDialog::~ColortableImportDialog()
{
    delete ui;
}


int ColortableImportDialog::redColumn(){

    return ui->sbRed->value();
}

int ColortableImportDialog::greenColumn(){

    return ui->sbGreen->value();
}

int ColortableImportDialog::blueColumn(){

    return ui->sbBlue->value();
}

int ColortableImportDialog::alphaColumn(){

    return ui->sbAlpha->value();
}

int ColortableImportDialog::minRed(){

    return ui->leMinRed->text().toInt();
}

int ColortableImportDialog::maxRed(){

    return ui->leMaxRed->text().toInt();
}

int ColortableImportDialog::minGreen(){

    return ui->leMinGreen->text().toInt();
}

int ColortableImportDialog::maxGreen(){

    return ui->leMaxGreen->text().toInt();
}

int ColortableImportDialog::minBlue(){

    return ui->leMinBlue->text().toInt();
}

int ColortableImportDialog::maxBlue(){

    return ui->leMaxBlue->text().toInt();
}

int ColortableImportDialog::minAlpha(){

    return ui->leMinAlpha->text().toInt();
}

int ColortableImportDialog::maxAlpha(){

    return ui->leMaxAlpha->text().toInt();
}


void ColortableImportDialog::setSkipLines( int n ){

    if( n==m_skipLines) return;

    if( n<0 ){
        qWarning( "Skiplines cannot be less than zero!");
        return;
    }

    m_skipLines=n;

    emit( skipLinesChanged(m_skipLines));
}


void ColortableImportDialog::setRedColumn( int col ){

    if( col < 0 ){
        qWarning( "Inline column cannot be less than one!");
        return;
    }

    ui->sbRed->setValue(col);

    updateLastUsedColumn();
}

void ColortableImportDialog::setGreenColumn( int col ){

    if( col < 0 ){
        qWarning( "Inline column cannot be less than one!");
        return;
    }

    ui->sbGreen->setValue(col);

    updateLastUsedColumn();
}

void ColortableImportDialog::setBlueColumn( int col ){

    if( col < 0 ){
        qWarning( "Inline column cannot be less than one!");
        return;
    }

    ui->sbBlue->setValue(col);

    updateLastUsedColumn();
}

void ColortableImportDialog::setAlphaColumn( int col ){

    if( col < 0 ){
        qWarning( "Inline column cannot be less than one!");
        return;
    }

    ui->sbAlpha->setValue(col);

    updateLastUsedColumn();
}



void ColortableImportDialog::setFilename(const QString & s){

    if( s==m_filename) return;

    m_filename=s;

    emit( filenameChanged(s) );
}

void ColortableImportDialog::setRedRange( int min, int max){


    if( min==minRed() && max==maxRed() ) return;

    if( min>max ){
        qWarning("Maximum red cannot be less than minimum!");
        return;
    }

    ui->leMinRed->setText( QString::number(min));
    ui->leMaxRed->setText( QString::number(max));

    emit( redRangeChanged(min,max));
}


void ColortableImportDialog::setGreenRange( int min, int max){


    if( min==minGreen() && max==maxGreen() ) return;

    if( min>max ){
        qWarning("Maximum green cannot be less than minimum!");
        return;
    }

    ui->leMinGreen->setText( QString::number(min));
    ui->leMaxGreen->setText( QString::number(max));

    emit( greenRangeChanged(min,max));
}


void ColortableImportDialog::setBlueRange( int min, int max){


    if( min==minBlue() && max==maxBlue() ) return;

    if( min>max ){
        qWarning("Maximum blue cannot be less than minimum!");
        return;
    }

    ui->leMinBlue->setText( QString::number(min));
    ui->leMaxBlue->setText( QString::number(max));

    emit( blueRangeChanged(min,max));
}

void ColortableImportDialog::setAlphaRange( int min, int max){


    if( min==minAlpha() && max==maxAlpha() ) return;

    if( min>max ){
        qWarning("Maximum alpha cannot be less than minimum!");
        return;
    }

    ui->leMinAlpha->setText( QString::number(min));
    ui->leMaxAlpha->setText( QString::number(max));

    emit( alphaRangeChanged(min,max));
}



void ColortableImportDialog::setColors(QVector<ColorTable::color_type> cols){

    if( cols==m_colors) return;

    m_colors=cols;

    emit colorsChanged(cols);
}

void ColortableImportDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Open Colortable file");

    if( fn.isNull()) return;        // cancelled

    setFilename(fn);
}


void ColortableImportDialog::updatePreview(const QString& filename)
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

void ColortableImportDialog::scan(){

    QFile file( m_filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Import Colortable"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(m_filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);



    int min_red=std::numeric_limits<int>::max();
    int max_red=std::numeric_limits<int>::min();

    int min_green=std::numeric_limits<int>::max();
    int max_green=std::numeric_limits<int>::min();

    int min_blue=std::numeric_limits<int>::max();
    int max_blue=std::numeric_limits<int>::min();

    int min_alpha=std::numeric_limits<int>::max();
    int max_alpha=std::numeric_limits<int>::min();


    int max=file.bytesAvailable();
    QProgressDialog pdlg("Scanning...", "Cancel", 0, max, this);
    //pdlg->show();
    int i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();
        int counter=max-file.bytesAvailable();

        if( i<=m_skipLines){
            continue;
        }

        int red;
        int green;
        int blue;
        int alpha;

        if( !splitLine(line, red, green, blue, alpha)){// continue;
            QMessageBox::warning(this, tr("Scan Horizon File"), QString("Error parsing line %1").arg(i) );
            return;
        }

        if( red<min_red ) min_red=red;
        if( red>max_red ) max_red=red;

        if( green<min_green ) min_green=green;
        if( green>max_green ) max_green=green;

        if( blue<min_blue ) min_blue=blue;
        if( blue>max_blue ) max_blue=blue;

        if( alpha<min_alpha ) min_alpha=alpha;
        if( alpha>max_alpha ) max_alpha=alpha;


        pdlg.setValue(counter);

        qApp->processEvents();

        if( pdlg.wasCanceled()){
            pdlg.setValue(max);
            return; // do not update min/max values
        }

    }

    pdlg.setValue(max);

    setRedRange( min_red, max_red);
    setGreenRange( min_green, max_green);
    setBlueRange( min_blue, max_blue);
    setAlphaRange( min_alpha, max_alpha);
}


void ColortableImportDialog::loadColors(){


    QVector<ColorTable::color_type> cols;

    QFile file( m_filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Import Colortable"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(m_filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    int max=file.bytesAvailable();
    QProgressDialog pdlg("Loading Colors...", "Cancel", 0, max, this);
    int i=0;

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();
        int counter=max-file.bytesAvailable();

        if( i<=m_skipLines){
            continue;
        }

        int red;
        int green;
        int blue;
        int alpha;

        if( !splitLine(line, red, green, blue, alpha)){
            QMessageBox::warning(this, tr("Import Horizon File"), QString("Error parsing line %1").arg(i) );
            return;
        }

        cols.append(qRgba(red, green, blue, alpha ));

        pdlg.setValue(counter);

        qApp->processEvents();

        if( pdlg.wasCanceled()){
            pdlg.setValue(max);
            return; // do not update min/max values
        }

    }

    pdlg.setValue(max);

    setColors(cols);
}

void ColortableImportDialog::accept(){

    loadColors();
    QDialog::accept();
}

void ColortableImportDialog::on_pbScan_clicked()
{
    scan();
}


bool ColortableImportDialog::channelFromString(QString str, int& c){

    double scaler=ui->cbScaler->currentText().toDouble();

    bool ok;

    c=0;

    double x=str.toDouble(&ok);
    if( !ok ) return false;
    x*=scaler;
    if( x<0 ) x=0;
    if( x>255 ) x=255;
    c=static_cast<int>(x);

    return true;
}


bool ColortableImportDialog::splitLine( const QString& line, int& red, int& green, int& blue, int& alpha){

    // need to cuild separator only once in calling function and check it there

     //QRegExp sep("(\\s+)");
     QRegExp sep(ui->cbSeparator->currentText() );
     if( !sep.isValid() ){
         //QMessageBox::critical(this, tr("Import Colortable"), tr("Invalid separator expression"));
         std::cout<<"Invalid regexp!!!"<<std::endl;
         return false;
     }

     QStringList col=line.split(sep, QString::SkipEmptyParts);

     double scaler=ui->cbScaler->currentText().toDouble();

     bool ok;

     if( redColumn()>col.size() ) return false;
     if( !channelFromString( col[redColumn()-1], red) ) return false;   // minimum 1, ui

     if( greenColumn()>col.size() ) return false;
     if( !channelFromString( col[greenColumn()-1], green) ) return false;   // minimum 1, ui

     if( blueColumn()>col.size() ) return false;
     if( !channelFromString( col[blueColumn()-1], blue) ) return false; // minimum 1, ui

     if( alphaColumn()>col.size() ) return false;
     if( alphaColumn() > 0 ){
         if( !channelFromString( col[alphaColumn()-1], alpha) ) return false;
     }
     else{      // alphaColumn==0, means no alpha, rgb a=255
         alpha=255;
     }

     return true;
}

 void ColortableImportDialog::connectToUI(){

     connect( this, SIGNAL( skipLinesChanged(int)), ui->sbSkip, SLOT(setValue(int)));
     connect( ui->sbSkip, SIGNAL(valueChanged(int)), this, SLOT( setSkipLines(int)));

     connect( ui->sbRed, SIGNAL(valueChanged(int)), this, SIGNAL(redColumnChanged(int)) );
     connect( ui->sbGreen, SIGNAL(valueChanged(int)), this, SIGNAL(greenColumnChanged(int)) );
     connect( ui->sbBlue, SIGNAL(valueChanged(int)), this, SIGNAL(blueColumnChanged(int)) );
     connect( ui->sbAlpha, SIGNAL(valueChanged(int)), this, SIGNAL(alphaColumnChanged(int)) );


     connect( this, SIGNAL( filenameChanged(QString)), this, SLOT( updatePreview(QString)));
     connect( this, SIGNAL( filenameChanged(QString)), ui->lePath, SLOT( setText(QString)));
 }

 void ColortableImportDialog::updateLastUsedColumn(){

     int l=redColumn();
     if( l<greenColumn()) l=greenColumn();
     if( l<blueColumn()) l=blueColumn();
     if( l<alphaColumn()) l=alphaColumn();
     m_lastUsedColumn=l;
 }
