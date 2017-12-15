#include "importgriddialog.h"
#include "ui_importgriddialog.h"

#include <grid2d.h>

#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include<iostream>
#include<gridformat.h>
#include<QProgressDialog>
#include<QStringList>


ImportGridDialog::ImportGridDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::ImportGridDialog)
{
    ui->setupUi(this);

    connect( ui->cbFormat, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFormat(QString)) );
    connect( ui->sbCoord1, SIGNAL(valueChanged(int)), this, SLOT( setCoord1Column(int)));
    connect( ui->sbCoord2, SIGNAL(valueChanged(int)), this, SLOT( setCoord2Column(int)));
    connect( ui->sbValue, SIGNAL( valueChanged(int)), this, SLOT( setValueColumn(int)));
    connect( ui->leNull, SIGNAL(textChanged(QString)), this, SLOT(setNullValue(QString)));
    connect( ui->leFile, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leName, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    ui->cbType->addItem(toQString(GridType::Horizon));
    ui->cbType->addItem(toQString(GridType::Attribute));
    ui->cbType->addItem(toQString(GridType::Other));

    ui->cbFormat->addItem(toQString(GridFormat::XYZ));
    ui->cbFormat->addItem(toQString(GridFormat::ILXLZ));

    updateCoordLabels();
}

ImportGridDialog::~ImportGridDialog()
{
    delete ui;
}

QMap<QString,QString> ImportGridDialog::params(){

    QMap<QString, QString> p;

    p.insert( "file", ui->leFile->text());
    p.insert( "type", ui->cbType->currentText());
    p.insert( "name", ui->leName->text());
    p.insert( "format", ui->cbFormat->currentText());
    p.insert( "null-value", ui->leNull->text() );
    p.insert( "coord1-column", QString::number(ui->sbCoord1->value()));
    p.insert( "coord2-column", QString::number(ui->sbCoord2->value()));
    p.insert( "value-column", QString::number(ui->sbValue->value()));
    p.insert( "skip-lines", QString::number(ui->sbSkip->value()));

    return p;
}


void ImportGridDialog::setProject(AVOProject * p){

    if( p==m_project) return;

    m_project=p;

    updateOkButton();
}

void ImportGridDialog::setType(GridType t){
    ui->cbType->setCurrentText(toQString(t));
}

void ImportGridDialog::setFormat(QString s){
    setFormat(toGridFormat(s));
}

void ImportGridDialog::setFormat(GridFormat f){

    if( f==m_format) return;

    m_format=f;

    updateCoordLabels();
}


// col is assumed 1-based
void ImportGridDialog::setCoord1Column( int col ){

    col--;

    if( col == m_coord1Column ) return;

    if( col < 0 ){
        qWarning( "Inline column cannot be less than zero!");
        return;
    }

    m_coord1Column=col;

    updateLastUsedColumn();
}

// col is assumed 1-based
void ImportGridDialog::setCoord2Column( int col ){

    col--;

    if( col == m_coord2Column ) return;

    if( col<0 ){
        qWarning( "Crossline column cannot be less than zero!");
        return;
    }

    m_coord2Column=col;

    updateLastUsedColumn();
}

// col is assumed 1-based
void ImportGridDialog::setValueColumn( int col ){

    col--;

    if( col == m_valueColumn ) return;

    if( col<0 ){
        qWarning( "Value column cannot be less than zero!");
        return;
    }

    m_valueColumn=col;

    updateLastUsedColumn();
}

void  ImportGridDialog::setNullValue( const QString& s ){

    if( s==m_nullValue ) return;

    m_nullValue = s;
}


void ImportGridDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Select grid file");

    if( fn.isNull()) return;        // cancelled

    ui->leFile->setText(fn);

    updatePreview(fn);
}


void ImportGridDialog::updatePreview(const QString& filename)
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

void ImportGridDialog::scan(){

    QString filename=ui->leFile->text();

    QFile file( filename );

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(filename)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);



    qreal min_coord1=std::numeric_limits<qreal>::max();
    qreal max_coord1=std::numeric_limits<qreal>::lowest();
    qreal min_coord2=std::numeric_limits<int>::max();
    qreal max_coord2=std::numeric_limits<int>::lowest();
    double min_value=std::numeric_limits<double>::max();
    double max_value=std::numeric_limits<double>::lowest();

    int max=file.bytesAvailable();
    QProgressDialog* pdlg=new QProgressDialog("Scanning Input File...", "Cancel", 0, max, this);
    //pdlg->show();
    int i=0;

    int skipLines=ui->sbSkip->value();

    while(!in.atEnd()){

        i++;

        QString line=in.readLine();
        int counter=max-file.bytesAvailable();

        if( i<=skipLines){
            continue;
        }

        qreal coord1;
        qreal coord2;
        double value;

        if( !splitLine(line, coord1, coord2, value)) continue;

        //std::cout<<iline<<" "<<xline<<" "<<value<<std::endl;

        if( coord1<min_coord1 ) min_coord1=coord1;
        if( coord1>max_coord1 ) max_coord1=coord1;

        if( coord2<min_coord2 ) min_coord2=coord2;
        if( coord2>max_coord2 ) max_coord2=coord2;


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

    ui->leMinCoord1->setText(QString::number(min_coord1));
    ui->leMaxCoord1->setText(QString::number(max_coord1));
    ui->leMinCoord2->setText(QString::number(min_coord2));
    ui->leMaxCoord2->setText(QString::number(max_coord2));
    ui->leMinValue->setText(QString::number(min_value));
    ui->leMaxValue->setText(QString::number(max_value));
}


void ImportGridDialog::on_pbScan_clicked()
{
    scan();
}

bool ImportGridDialog::splitLine( const QString& line, qreal& coord1, qreal& coord2, double& value){


     QRegExp sep("(\\s+)");

     QStringList col=line.split(sep, QString::SkipEmptyParts);

//       std::cout<<col.size()<<" "<<m_inlineColumn <<" "<<m_crosslineColumn<<" "<<m_valueColumn<<" "<<m_lastUsedColumn<<std::endl;


     if( col.size()<=m_lastUsedColumn){
        return false;
     }

     if( m_nullValue==col[m_valueColumn] ){
         return false;
     }

     bool ok;
     coord1=col[m_coord1Column].toDouble(&ok);
     if( !ok ) return false;

     coord2=col[m_coord2Column].toDouble(&ok);
     if( !ok ) return false;

     value=col[m_valueColumn].toDouble(&ok);
     if( !ok ) return false;

     return true;
}


 void ImportGridDialog::updateLastUsedColumn(){

     int l=m_coord1Column;
     if( l<m_coord2Column) l=m_coord2Column;
     if( l<m_valueColumn) l=m_valueColumn;
     m_lastUsedColumn=l;
 }


 void ImportGridDialog::updateCoordLabels(){

     switch( m_format ){
     case GridFormat::XYZ:
         ui->lbCoord1->setText("X:");
         ui->lbCoord2->setText("Y:");
         break;
     case GridFormat::ILXLZ:
         ui->lbCoord1->setText("Inline:");
         ui->lbCoord2->setText("Xline:");
         break;
     default:
         qFatal("Unhandled format in ImportGridDialog!");
         break;
     }

 }

 void ImportGridDialog::updateOkButton(){

     if(!m_project) return;

     bool ok=true;

     if( ui->leFile->text().isEmpty()) ok=false;

     auto otype=toGridType(ui->cbType->currentText());
     auto oname=ui->leName->text();
     QPalette palette;
     if( oname.isEmpty() || m_project->existsGrid(otype, oname) ){    // make sure not to call existsgrid with empty name!!!
         ok=false;
         palette.setColor(QPalette::Text, Qt::red);
     }
     ui->leName->setPalette(palette);

     ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
 }
