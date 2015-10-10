#include "segyinputdialog.h"
#include "ui_segyinputdialog.h"

#include<iostream>
#include<iomanip>
#include<fstream>
#include<chrono>

#include<cmath>  // fabs

#include <ebcdicdialog.h>
#include <headerdialog.h>
#include<segyreader.h>
#include <navigationwidget.h>

#include<QFileDialog>
#include<QProgressDialog>
#include<QMessageBox>

using seismic::HeaderValue;


SegyInputDialog::SegyInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SegyInputDialog)
{
    ui->setupUi(this);

    ui->wdAdvanced->hide();

    QValidator* posValidator=new QIntValidator(1, 240, this);
    ui->lePosCDP->setValidator( posValidator );
    ui->lePosIline->setValidator( posValidator );
    ui->lePosXline->setValidator( posValidator );
    ui->lePosCDPX->setValidator( posValidator );
    ui->lePosCDPY->setValidator( posValidator );
    ui->lePosOffset->setValidator( posValidator );

    QStringList items;
    items.append("2");
    items.append("4");
    ui->cbSizeCDP->addItems(items);
    ui->cbSizeIline->addItems(items);
    ui->cbSizeXline->addItems(items);
    ui->cbSizeOffset->addItems(items);
    ui->cbSizeCDPX->addItems(items);
    ui->cbSizeCDPY->addItems(items);
    ui->cbSizeOffset->addItems(items);

    ui->lePosCDP->setText("21");
    ui->cbSizeCDP->setCurrentIndex(1);

    ui->lePosCDPX->setText("181");
    ui->cbSizeCDPX->setCurrentIndex(1);

    ui->lePosCDPY->setText("185");
    ui->cbSizeCDPY->setCurrentIndex(1);

    ui->lePosIline->setText("189");
    ui->cbSizeIline->setCurrentIndex(1);

    ui->lePosXline->setText("193");
    ui->cbSizeXline->setCurrentIndex(1);

    ui->lePosOffset->setText("37");
    ui->cbSizeOffset->setCurrentIndex(1);

    QStringList format_items;
    format_items.append("From Header");
    format_items.append("IBM");
    format_items.append("IEEE");
    format_items.append("4 Byte Integer");
    format_items.append("2 Byte Integer");
    format_items.append("1 Byte Integer");
    ui->cbFormat->addItems(format_items);
    ui->cbFormat->setCurrentIndex(0);


    // have to put those here because qtcreator doesn't allow to connect signals with argument to slots with no args even though this should be legal!
    connect( ui->lePosCDP, SIGNAL(textChanged(QString)), this, SLOT(readerParamsChanged()));
    connect( ui->lePosCDPX, SIGNAL(textChanged(QString)), this, SLOT(readerParamsChanged()));
    connect( ui->lePosCDPY, SIGNAL(textChanged(QString)), this, SLOT(readerParamsChanged()));
    connect( ui->lePosIline, SIGNAL(textChanged(QString)), this, SLOT(readerParamsChanged()));
    connect( ui->lePosXline, SIGNAL(textChanged(QString)), this, SLOT(readerParamsChanged()));
    connect( ui->lePosOffset, SIGNAL(textChanged(QString)), this, SLOT(readerParamsChanged()));
    connect( ui->leScalco, SIGNAL(textChanged(QString)), this, SLOT(readerParamsChanged()));
    connect( ui->leScalel, SIGNAL(textChanged(QString)), this, SLOT(readerParamsChanged()));
    connect( ui->cbSizeCDP, SIGNAL(currentIndexChanged(int)), this, SLOT(readerParamsChanged()));
    connect( ui->cbSizeCDPX, SIGNAL(currentIndexChanged(int)), this, SLOT(readerParamsChanged()));
    connect( ui->cbSizeCDPY, SIGNAL(currentIndexChanged(int)), this, SLOT(readerParamsChanged()));
    connect( ui->cbSizeIline, SIGNAL(currentIndexChanged(int)), this, SLOT(readerParamsChanged()));
    connect( ui->cbSizeXline, SIGNAL(currentIndexChanged(int)), this, SLOT(readerParamsChanged()));
    connect( ui->cbSizeOffset, SIGNAL(currentIndexChanged(int)), this, SLOT(readerParamsChanged()));
    connect( ui->cbFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(readerParamsChanged()));
    connect( ui->rbScalcoHeader, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()));
    connect( ui->rbScalelHeader, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()));
    connect( ui->rbSwapBytes, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()));

    updateButtons();
}

SegyInputDialog::~SegyInputDialog()
{
    delete ui;
    if(m_reader) delete m_reader;
}

void SegyInputDialog::on_btScan_clicked()
{
    //std::cout<<"SCANNING..."<<std::endl;
    Q_ASSERT(m_reader);


    std::vector<seismic::SEGYHeaderWordDef> orig_def=m_reader->trace_header_def();
    std::vector<seismic::SEGYHeaderWordDef> tmp_def;
    for( auto& d : orig_def){
        if( d.name == "iline" ||
                d.name=="xline" ||
                d.name=="cdp" ||
                d.name=="cdpx" ||
                d.name=="cdpy" ||
                d.name=="offset" ||
                d.name=="scalco" ||
                d.name=="ns" ){
                tmp_def.push_back(d);
            }
        }

        m_reader->set_trace_header_def(tmp_def);

    try{


        QProgressDialog* pdlg=new QProgressDialog("Scanning Trace Headers", "Cancel", 0, m_reader->trace_count(), this);

        HeaderValue min_iline(HeaderValue::int_type(1));
        HeaderValue max_iline(HeaderValue::int_type(-1));

        HeaderValue min_xline(HeaderValue::int_type(1));
        HeaderValue max_xline(HeaderValue::int_type(-1));

        HeaderValue min_cdp(HeaderValue::int_type(1));
        HeaderValue max_cdp(HeaderValue::int_type(-1));

        HeaderValue min_offset(HeaderValue::float_type(1));
        HeaderValue max_offset(HeaderValue::float_type(-1));

        HeaderValue min_cdpx(HeaderValue::float_type(1.));
        HeaderValue max_cdpx(HeaderValue::float_type(-1.));

        HeaderValue min_cdpy(HeaderValue::float_type(1.));
        HeaderValue max_cdpy(HeaderValue::float_type(-1.));

         auto start = std::chrono::steady_clock::now();

        for( int i=0; i<m_reader->trace_count(); i++){

            m_reader->seek_trace(i);
            seismic::Header th=m_reader->read_trace_header();

            HeaderValue cdp=th["cdp"];
            HeaderValue cdpx=th["cdpx"];
            HeaderValue cdpy=th["cdpy"];
            HeaderValue iline=th["iline"];
            HeaderValue xline=th["xline"];
            HeaderValue offset=th["offset"];

            if( i==0 ){
                min_iline=max_iline=iline;
                min_xline=max_xline=xline;
                min_cdp=max_cdp=cdp;
                min_cdpx=max_cdpx=cdpx;
                min_cdpy=max_cdpy=cdpy;
                min_offset=max_offset=offset;
            }

            if( cdp<min_cdp ) min_cdp=cdp;
            else if( max_cdp<cdp ) max_cdp=cdp;

            if( cdpx<min_cdpx ) min_cdpx=cdpx;
            else if( max_cdpx<cdpx ) max_cdpx=cdpx;

            if( cdpy<min_cdpy ) min_cdpy=cdpy;
            else if( max_cdpy<max_cdpy ) max_cdpy=cdpy;

            if( iline<min_iline ) min_iline=iline;
            else if( max_iline<max_iline ) max_iline=iline;

            if( xline<min_xline ) min_xline=xline;
            else if( max_xline<max_xline ) max_xline=xline;

            if( offset<min_offset ) min_offset=offset;
            else if( max_offset<offset ) max_offset=offset;

            pdlg->setValue(i+1);
            qApp->processEvents();
            if( pdlg->wasCanceled()){
                pdlg->setValue(m_reader->trace_count());
                return; // do not update min/max values
            }
        }

        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout<<"Scanning took "<< std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

        ui->leMinCDP->setText( QString::number(min_cdp.intValue()));
        ui->leMaxCDP->setText( QString::number(max_cdp.intValue()));

        ui->leMinCDPX->setText( QString::number(min_cdpx.floatValue()));
        ui->leMaxCDPX->setText( QString::number(max_cdpx.floatValue()));

        ui->leMinCDPY->setText( QString::number(min_cdpy.floatValue()));
        ui->leMaxCDPY->setText( QString::number(max_cdpy.floatValue()));

        ui->leMinOffset->setText( QString::number(min_offset.floatValue()));
        ui->leMaxOffset->setText( QString::number(max_offset.floatValue()));

        ui->leMinIline->setText( QString::number(min_iline.intValue()));
        ui->leMaxIline->setText( QString::number(max_iline.intValue()));

        ui->leMinXline->setText( QString::number(min_xline.intValue()));
        ui->leMaxXline->setText( QString::number(max_xline.intValue()));

    }
    catch(seismic::SEGYReader::FormatError& err){
        QMessageBox::critical(this, "Scanning SEGY-file", QString("Exception occured:\n")+QString(err.what()));
        destroyReader();
    }

        if( m_reader) m_reader->set_trace_header_def(orig_def);

}

void SegyInputDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Open SEGY file");
    if( fn.isNull()) return;        // cancelled
    ui->leFilename->setText(fn);    // filename always synchronized with lineedit
    openReader();                    // openReader uses filename from lineedit
}

void SegyInputDialog::on_pbEBCDIC_clicked()
{
    Q_ASSERT(m_reader);

    if( !m_EBCDICDialog ){
        m_EBCDICDialog=new EBCDICDialog(this);
    }

    updateEBCDICDialog();
}


void SegyInputDialog::on_pbBinary_clicked()
{
    Q_ASSERT(m_reader);

    if( !m_BinaryDialog ){
        m_BinaryDialog=new HeaderDialog(this);
    }

    updateBinaryDialog();
}


void SegyInputDialog::on_pbTraceHeader_clicked()
{
    Q_ASSERT(m_reader);

    if( !m_TraceHeaderDialog ){
        m_TraceHeaderDialog=new HeaderDialog(this, true);
         m_TraceHeaderDialog->navigationWidget()->setRange(1, m_reader->trace_count());
         m_TraceHeaderDialog->show();
        connect( m_TraceHeaderDialog->navigationWidget(), SIGNAL( currentChanged(int)), this, SLOT(navigateToTraceHeader(int)));
        connect( m_TraceHeaderDialog, SIGNAL(finished(int)), this, SLOT(onTraceHeaderDialogFinished()));
        m_TraceHeaderDialog->navigationWidget()->setCurrent(1);
        navigateToTraceHeader( 1 ); // have to add this because NavigationWidget is posisionted at 1 and won't emit signal
    }
}

void SegyInputDialog::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
        apply();
        //std::cout<<"ATE IT"<<std::endl;
    }
    else{
        QDialog::keyPressEvent(ev);
    }

}

void SegyInputDialog::readerParamsChanged(){
    openReader();

    if( m_TraceHeaderDialog ){
        navigateToTraceHeader( m_TraceHeaderDialog->navigationWidget()->current() );    // reload trace header with adjusted reader params
    }
}


void SegyInputDialog::openReader(){


      // close already open reader
      if( m_reader ){
          delete m_reader;
          m_reader=nullptr;
      }

      try{

          // get parameters from controls:
          bool swap=ui->rbSwapBytes->isChecked();

          bool override_format=( ui->cbFormat->currentIndex()>0);

          seismic::SEGYReader::SampleFormat format;
          switch(int idx=ui->cbFormat->currentIndex() ){
              case 0: break;                                                // from header, no need for change
              case 1: format=seismic::SEGYReader::SampleFormat::IBM; break;
              case 2: format=seismic::SEGYReader::SampleFormat::IEEE; break;
              case 3: format=seismic::SEGYReader::SampleFormat::INT4; break;
              case 4: format=seismic::SEGYReader::SampleFormat::INT2; break;
              case 5: format=seismic::SEGYReader::SampleFormat::INT1; break;
              default: qFatal(QString("Invalid index (%1) for sample format!").arg(idx).toStdString().c_str()); break;
          }

          std::vector<seismic::SEGYHeaderWordDef> thdef=getTraceHeaderDefinition();

          // open a reader
          seismic::SEGYReader* reader=new seismic::SEGYReader(ui->leFilename->text().toStdString(), swap);

          if( override_format ){
              reader->set_sample_format( format );  // can do this after open and reading of binary header because it only has effect for traces
          }

          reader->set_trace_header_def( thdef );    // set trace header definitions, can do this now because no trace header has been read yet

          if( ui->rbScalcoFixed->isChecked()){
              reader->set_fixed_scalco(true);
              reader->set_scalco(ui->leScalco->text().toDouble());
          }

          if( ui->rbScalelFixed->isChecked()){
              reader->set_fixed_scalel(true);
              reader->set_scalel(ui->leScalel->text().toDouble());
          }

          // everything ok, keep reader
          m_reader=reader;
      }
      catch(seismic::SEGYReader::FormatError& err){
          QMessageBox::critical(this, "Open SEGY-file", QString("Exception occured:\n")+QString(err.what()));
           ui->leFilename->setText("");
      }
      catch( ... ){
          qFatal("EXCEPTION");
      }

      updateButtons();
}


void SegyInputDialog::destroyReader(){
    delete m_reader;
    m_reader=nullptr;
    updateButtons();
}

void SegyInputDialog::updateButtons(){
      bool on=m_reader;
      ui->btScan->setEnabled(on);
      ui->pbEBCDIC->setEnabled(on);
      ui->pbBinary->setEnabled(on);
      ui->pbTraceHeader->setEnabled(on);
      ui->pbOK->setEnabled(on);
}

void SegyInputDialog::updateEBCDICDialog(){

    Q_ASSERT( m_EBCDICDialog );

    m_EBCDICDialog->setHeader(m_reader->textHeaders()[0]);

    m_EBCDICDialog->setWindowTitle( QString( "Textual Header of \"") + ui->leFilename->text() + QString("\""));

    m_EBCDICDialog->show();
}

void SegyInputDialog::updateBinaryDialog(){

    Q_ASSERT( m_BinaryDialog );

    m_BinaryDialog->setData(m_reader->binaryHeader(), m_reader->binary_header_def());

    m_BinaryDialog->setWindowTitle( QString( "Binary Header of \"") + ui->leFilename->text() + QString("\""));

    m_BinaryDialog->show();
}

void SegyInputDialog::onTraceHeaderDialogFinished(){
    m_TraceHeaderDialog=nullptr;
}

// n is one based, READER TRACE NUMBERS ARE 0 BASED!
 void SegyInputDialog::navigateToTraceHeader( int n ){

     Q_ASSERT( m_reader );

    try{

         n--;

        m_reader->seek_trace(n);

        seismic::Header hdr=m_reader->read_trace_header();

        // repos. stream at beg of header for next read!
        m_reader->seek_trace(n);

        m_TraceHeaderDialog->setData( hdr, m_reader->trace_header_def());

        m_TraceHeaderDialog->setWindowTitle( QString( "Trace Header of \"") + ui->leFilename->text() + QString("\""));
    }
    catch( seismic::SEGYReader::FormatError& err ){
        QMessageBox::critical(this, "Scanning SEGY-file", QString("Exception occured:\n")+QString(err.what()));
        destroyReader();
    }
}


std::vector<seismic::SEGYHeaderWordDef> SegyInputDialog::getTraceHeaderDefinition(){

    std::vector<seismic::SEGYHeaderWordDef> thdef = seismic::SEGY_DEFAULT_TRACE_HEADER_DEF;
    for( auto& d : thdef){
        if( d.name == "iline"){
            d.pos=ui->lePosIline->text().toUInt();
            d.dtype=(ui->cbSizeIline->currentIndex()==1)?seismic::SEGY_INT32 : seismic::SEGY_INT16;
        }
        else if( d.name == "xline"){
            d.pos=ui->lePosXline->text().toUInt();
            d.dtype=(ui->cbSizeXline->currentIndex()==1)?seismic::SEGY_INT32 : seismic::SEGY_INT16;
        }
        else if( d.name == "cdp"){
            d.pos=ui->lePosCDP->text().toUInt();
            d.dtype=(ui->cbSizeCDP->currentIndex()==1)?seismic::SEGY_INT32 : seismic::SEGY_INT16;
        }
        else if( d.name == "cdpx"){
            d.pos=ui->lePosCDPX->text().toUInt();
            d.dtype=(ui->cbSizeCDPX->currentIndex()==1)?seismic::SEGY_INT32 : seismic::SEGY_INT16;
        }
        else if( d.name == "cdpy"){
            d.pos=ui->lePosCDPY->text().toUInt();
            d.dtype=(ui->cbSizeCDPY->currentIndex()==1)?seismic::SEGY_INT32 : seismic::SEGY_INT16;
        }
        else if( d.name == "offset"){
            d.pos=ui->lePosOffset->text().toUInt();
            d.dtype=(ui->cbSizeOffset->currentIndex()==1)?seismic::SEGY_INT32 : seismic::SEGY_INT16;
        }

        // check for out of bounds header word pos
        size_t end=d.pos+( ( d.dtype==seismic::SEGY_INT16 ) ? 2 : 4 );
        //std::cout<<d.name<<" end="<<end<<std::endl;
        if( end >240 ){   // 1 based
            QMessageBox::critical( this, "Change SEGY trace header definition", QString("Header word \"") + QString(d.name.c_str()) + QString("\" is out of trace header bounds!") );
            thdef.clear();          // return empty vector on error!!!
        }
    }

    return thdef;
}

void SegyInputDialog::apply(){
    openReader();

    if( m_TraceHeaderDialog ){
        navigateToTraceHeader( m_TraceHeaderDialog->navigationWidget()->current() );    // reload trace header with adjusted reader params
    }
}

void SegyInputDialog::on_leFilename_returnPressed()
{
    openReader();
}

void SegyInputDialog::on_pbOK_clicked()
{
    // be sure to catch the most recent changes
    openReader();
    if( m_reader ) accept();
}
