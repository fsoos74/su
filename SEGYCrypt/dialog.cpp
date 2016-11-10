#include "dialog.h"
#include "ui_dialog.h"

#include<QFileDialog>
#include<QProgressDialog>
#include<QMessageBox>


#include<segyreader.h>
#include<segywriter.h>

#include<crypt.h>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pbGo_clicked()
{

    QString iname=ui->leInput->text();
    QString oname=ui->leOutput->text();

    if( iname.isEmpty()){
        QMessageBox::critical(this, tr("Convert File"), tr("No input file selected"));
        return;
    }

    if( oname.isEmpty()){
        QMessageBox::critical(this, tr("Convert File"), tr("No output file selected"));
        return;
    }

    if( ui->cbMode->currentIndex()==0){     // encrypt
        encrypt(iname, oname);
    }
    else if( ui->cbMode->currentIndex()==1){    // decrypt
        decrypt(iname, oname);
    }
    else{
        qFatal("ILLEGAL MODE");
    }
}

void Dialog::on_pbBrowseInput_clicked()
{
    QString name=QFileDialog::getOpenFileName(this, tr("Select Input File"), QDir::homePath(), "*");
    if( name.isNull() ) return;

    ui->leInput->setText(name);

}

void Dialog::on_pbBrowseOutput_clicked()
{
    QString name=QFileDialog::getSaveFileName(this, tr("Select Output File"), QDir::homePath(), "*");
    if( name.isNull() ) return;

    ui->leOutput->setText(name);
}


// CURRENTLY THIS CUTS THE OUTPUT TRACES!!!
void Dialog::encrypt(QString iname, QString oname ){

    try{

    seismic::SEGYInfo info;     // all defaults

    seismic::SEGYReader reader( iname.toStdString() , info);

    const int NT=1251;

    seismic::SEGYWriter::preWriteFunc=crypt::encrypt;       // need to set this before creating writer!!!
    info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);  // currently only ieee support
    seismic::Header bhdr=reader.binaryHeader();
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );
    bhdr["ns"]=seismic::HeaderValue::makeUIntValue(NT);
    seismic::SEGYWriter writer( oname.toStdString(), info,
                                reader.textHeaders()[0], bhdr);

    writer.write_leading_headers();

    QProgressDialog progress(tr("encrypt file"), tr("Cancel"), 0, reader.trace_count() );
    size_t step=reader.trace_count();
    if( step>100) step=step/100;        // greater than zero

    for( size_t i=0; i<reader.trace_count(); i++){

        reader.seek_trace(i);
        seismic::Trace trace=reader.read_trace();

        seismic::Trace::Samples sam(NT);
        for( int i=0; i<NT; i++){
            sam[i]=trace.samples()[i];
        }
        seismic::Header hdr=trace.header();
        hdr["ns"]=seismic::HeaderValue::makeUIntValue(NT);

        seismic::Trace otrace(trace.ft(), trace.dt(), sam, hdr);

        writer.write_trace(otrace);
        progress.setValue(i);

        if( (i % step)==0 ){
            qApp->processEvents();
            if( progress.wasCanceled()){
                QMessageBox::information(this, tr("encrypt file"), tr("Cancelled"));
                break;
            }
        }
    }

    }
    catch(seismic::SEGYFormatError& err){
        QMessageBox::critical(this, tr("encrypt file"), err.what());
    }
}

void Dialog::decrypt(QString iname, QString oname){

    try{

    seismic::SEGYInfo info;     // all defaults

    seismic::SEGYReader::postReadFunc=crypt::decrypt;

    seismic::SEGYReader reader( iname.toStdString() , info);

    info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);  // currently only ieee support
    seismic::Header bhdr=reader.binaryHeader();
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );
    seismic::SEGYWriter writer( oname.toStdString(), info,
                                reader.textHeaders()[0], bhdr);

    writer.write_leading_headers();

    QProgressDialog progress(tr("decrypt file"), tr("Cancel"), 0, reader.trace_count() );
    size_t step=reader.trace_count();
    if( step>100) step=step/100;        // greater than zero

    for( size_t i=0; i<reader.trace_count(); i++){

        reader.seek_trace(i);
        seismic::Trace trace=reader.read_trace();
        writer.write_trace(trace);
        progress.setValue(i);

        if( (i % step)==0 ){
            qApp->processEvents();
            if( progress.wasCanceled()){
                QMessageBox::information(this, tr("decrypt file"), tr("Cancelled"));
                break;
            }
        }
    }

    }
    catch(seismic::SEGYFormatError& err){
        QMessageBox::critical(this, tr("encrypt file"), err.what());
    }
}

