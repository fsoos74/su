#include "segyinputdialog.h"
#include "ui_segyinputdialog.h"

#include<iostream>
#include<iomanip>
#include<fstream>
#include<chrono>

#include<cmath>  // fabs
#include<cstring> //memcpy
#include<headerscandialog.h>
#include<header.h>
#include <ebcdicdialog.h>
#include <headerdialog.h>
#include<segyreader.h>
#include<seis_bits.h>
#include<segy_header_def.h>
#include <xsiwriter.h>
#include<xsireader.h>
#include <navigationwidget.h>

#include<QFileDialog>
#include<QProgressDialog>
#include<QMessageBox>

using seismic::HeaderValue;
using seismic::SEGYHeaderWordConvType;

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

    QDoubleValidator* dvalidator=new QDoubleValidator(this);
    ui->leScalco->setValidator(dvalidator);

    QStringList items;
    items.append("2 byte integer");     // index = 0
    items.append("4 byte integer");     // index = 1

    ui->cbSizeCDP->addItems(items);
    ui->cbSizeIline->addItems(items);
    ui->cbSizeXline->addItems(items);

    items.append("ieee floating point"); // index = 2
    ui->cbSizeCDPX->addItems(items);
    ui->cbSizeCDPY->addItems(items);
    ui->cbSizeOffset->addItems(items);

    QStringList format_items;
    format_items.append("From Header");
    format_items.append("IBM");
    format_items.append("IEEE");
    format_items.append("4 Byte Integer");
    format_items.append("2 Byte Integer");
    format_items.append("1 Byte Integer");
    ui->cbFormat->addItems(format_items);
    ui->cbFormat->setCurrentIndex(0);

    updateControlsFromInfo();


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
    connect( ui->cbUseScalco, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()));
    connect( ui->leScalco, SIGNAL(returnPressed()), this, SLOT(readerParamsChanged()));
    connect( ui->cbScaleOffsets, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()) );
    connect( ui->cbUseScalel, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()));
    connect( ui->leScalel, SIGNAL(returnPressed()), this, SLOT(readerParamsChanged()));
    connect( ui->cbSwapBytes, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()));
    connect( ui->cbForceFixedSampling, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()));
    connect( ui->cbUseNEXT, SIGNAL(toggled(bool)), this, SLOT(readerParamsChanged()));

    updateButtons();
}

SegyInputDialog::~SegyInputDialog()
{
    delete ui;
}

QString SegyInputDialog::path()const
{
    return ui->leFilename->text();
}

void SegyInputDialog::setInfo(const seismic::SEGYInfo &info){


    m_info=info;

    updateControlsFromInfo();
}

void SegyInputDialog::setPrestack(bool on){

    if( on==m_prestack) return;

    m_prestack=on;

    ui->lbOffset->setEnabled(on);
    ui->leMinOffset->setEnabled(on);
    ui->leMaxOffset->setEnabled(on);
    ui->lePosOffset->setEnabled(on);
    ui->cbSizeOffset->setEnabled(on);
}

void SegyInputDialog::set3D(bool on){

    if( on==m_3d) return;

    m_3d=on;

    ui->lbIline->setEnabled(on);
    ui->lePosIline->setEnabled(on);
    ui->cbSizeIline->setEnabled(on);
    ui->leMinIline->setEnabled(on);
    ui->leMaxIline->setEnabled(on);

    ui->lbXline->setEnabled(on);
    ui->lePosXline->setEnabled(on);
    ui->cbSizeXline->setEnabled(on);
    ui->leMinXline->setEnabled(on);
    ui->leMaxXline->setEnabled(on);
}

void SegyInputDialog::updateControlsFromInfo(){


    const std::vector<seismic::SEGYHeaderWordDef>& defs=m_info.traceHeaderDef();

    m_updateReaderParamsEnabled=false;

    for( auto def:defs){

        //std::cout<<"!!!"<<def.name<<std::endl;

        QLineEdit* lePos=nullptr;
        QComboBox* cbSize=nullptr;


        if(def.name=="iline" && m_3d){
            lePos=ui->lePosIline;
            cbSize=ui->cbSizeIline;
        }
        else if(def.name=="xline" && m_3d){
            lePos=ui->lePosXline;
            cbSize=ui->cbSizeXline;
        }
        else if(def.name=="cdp"){
            lePos=ui->lePosCDP;
            cbSize=ui->cbSizeCDP;
        }

        else if(def.name=="cdpx"){
            lePos=ui->lePosCDPX;
            cbSize=ui->cbSizeCDPX;
        }
        else if(def.name=="cdpy"){
            lePos=ui->lePosCDPY;
            cbSize=ui->cbSizeCDPY;
        }
        else if(def.name=="offset" && m_prestack){
            lePos=ui->lePosOffset;
            cbSize=ui->cbSizeOffset;
            ui->cbScaleOffsets->setChecked( def.ctype==SEGYHeaderWordConvType::COORD );
        }


        if( (lePos) && (cbSize) ){
            std::cout<<"SET TO CONTROLS: "<<def.name<<" pos="<<def.pos<<" type="
            <<seismic::toQString(def.dtype).toStdString()<<"=="<< DataTypeToIndex(def.dtype)<<std::endl;

            lePos->setText(QString::number(def.pos));
            cbSize->setCurrentIndex( DataTypeToIndex(def.dtype) );
        }
    }

    ui->cbSwapBytes->setChecked(m_info.isSwap());

    bool forceFixed=m_info.isFixedSampling() && m_info.isOverrideFixedSampling();
    ui->cbForceFixedSampling->setChecked(forceFixed);

    int index=0;
    if( m_info.isOverrideSampleFormat()){

        switch(m_info.sampleFormat()){
        case seismic::SEGYSampleFormat::IBM: index=1;break;
        case seismic::SEGYSampleFormat::IEEE: index=2;break;
        case seismic::SEGYSampleFormat::INT4: index=3;break;
        case seismic::SEGYSampleFormat::INT2: index=4;break;
        case seismic::SEGYSampleFormat::INT1: index=5;break;
        default: qFatal("Illegal sampleformat!");break;
        }
    }
    ui->cbFormat->setCurrentIndex(index);

    ui->cbUseNEXT->setChecked(!m_info.isOverrideNEXT());
    ui->sbNEXT->setValue(m_info.next());

    ui->cbUseScalco->setChecked(!m_info.isFixedScalco());
    if( m_info.isFixedScalco()){
        ui->leScalco->setText(QString::number(m_info.scalco()));
    }

    ui->cbUseScalel->setChecked(!m_info.isFixedScalel());
    if( m_info.isFixedScalel()){
        ui->leScalel->setText(QString::number(m_info.scalel()));
    }

    m_updateReaderParamsEnabled=true;

    openReader();
}

void SegyInputDialog::updateInfoFromControls(){

    std::vector<seismic::SEGYHeaderWordDef> hdefs=m_info.traceHeaderDef();

    for( auto& def : hdefs){

        QLineEdit* lePos=nullptr;
        QComboBox* cbSize=nullptr;

        if(def.name=="iline" && m_3d){
            lePos=ui->lePosIline;
            cbSize=ui->cbSizeIline;
        }
        else if(def.name=="xline" && m_3d){
            lePos=ui->lePosXline;
            cbSize=ui->cbSizeXline;
        }
        else if(def.name=="cdp"){
            lePos=ui->lePosCDP;
            cbSize=ui->cbSizeCDP;
        }
        else if(def.name=="cdpx"){
            lePos=ui->lePosCDPX;
            cbSize=ui->cbSizeCDPX;
        }
        else if(def.name=="cdpy"){
            lePos=ui->lePosCDPY;
            cbSize=ui->cbSizeCDPY;
        }
        else if(def.name=="offset" && m_prestack){
            lePos=ui->lePosOffset;
            cbSize=ui->cbSizeOffset;
            def.ctype=(ui->cbScaleOffsets->isChecked())?
                        SEGYHeaderWordConvType::COORD : SEGYHeaderWordConvType::FLOAT;
        }

        if( lePos && cbSize ){
            def.pos=lePos->text().toUInt();
            def.dtype=indexToDataType(cbSize->currentIndex());
        }

    }

    m_info.setTraceHeaderDef(hdefs);

    int index=ui->cbFormat->currentIndex();
    switch(index){
    case 0: m_info.setOverrideSampleFormat(false);break;          // no format predefined
    case 1: m_info.setSampleFormat(seismic::SEGYSampleFormat::IBM);break;
    case 2: m_info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);break;
    case 3: m_info.setSampleFormat( seismic::SEGYSampleFormat::INT4);break;
    case 4: m_info.setSampleFormat( seismic::SEGYSampleFormat::INT2);break;
    case 5: m_info.setSampleFormat( seismic::SEGYSampleFormat::INT1);break;
    default: qFatal("Illegal sampleformat!");break;
    }

    if( ui->cbUseScalco->isChecked()){
        m_info.setFixedScalco(false);
    }
    else{
        m_info.setFixedScalco(true);
        m_info.setScalco(ui->leScalco->text().toDouble());
    }

    if( ui->cbUseScalel->isChecked()){
        m_info.setFixedScalel(false);
    }
    else{
        m_info.setFixedScalel(true);
        m_info.setScalel(ui->leScalel->text().toDouble());
    }

    if( ui->cbUseNEXT->isChecked()){
        m_info.setOverrideNEXT(false);
    }
    else{
        m_info.setOverrideNEXT(true);
        m_info.setNEXT(ui->sbNEXT->value());
    }

    m_info.setSwap(ui->cbSwapBytes->isChecked());
    m_info.setOverrideFixedSampling(ui->cbForceFixedSampling->isChecked());
    m_info.setFixedSamling(ui->cbForceFixedSampling->isChecked());
    //m_infoDirty=true;
}

void SegyInputDialog::on_btScan_clicked()
{
    //std::cout<<"SCANNING..."<<std::endl;
    Q_ASSERT(m_reader);

    seismic::SEGYInfo orig_info=m_reader->info();

    // using only necessary header words to speed up scan
    std::vector<seismic::SEGYHeaderWordDef> tmp_def;
    for( auto& d : orig_info.traceHeaderDef()){
        if( d.name == "iline" || d.name=="xline" ||
                d.name=="cdp" ||
                d.name=="cdpx" ||
                d.name=="cdpy" ||
                d.name=="offset" ||
                d.name=="scalco" ||
                d.name=="ns" ||
                d.name=="dt" ){ // need dt here, even if not used, missing would result in error in convert trace header!!!
                tmp_def.push_back(d);
            }
        }

    seismic::SEGYInfo tmp_info(orig_info);
    tmp_info.setTraceHeaderDef(tmp_def);
        m_reader->setInfo(tmp_info);

    try{


        QProgressDialog* pdlg=new QProgressDialog("Scanning Trace Headers", "Cancel", 0, m_reader->trace_count(), this);

        HeaderValue min_iline(HeaderValue::int_type(1));
        HeaderValue max_iline(HeaderValue::int_type(-1));

        HeaderValue min_xline(HeaderValue::int_type(1));
        HeaderValue max_xline(HeaderValue::int_type(-1));

        HeaderValue min_cdp(HeaderValue::int_type(1));
        HeaderValue max_cdp(HeaderValue::int_type(-1));

        HeaderValue min_offset(HeaderValue::int_type(1));
        HeaderValue max_offset(HeaderValue::int_type(-1));

        HeaderValue min_cdpx(HeaderValue::float_type(1));
        HeaderValue max_cdpx(HeaderValue::float_type(-1));

        HeaderValue min_cdpy(HeaderValue::float_type(1));
        HeaderValue max_cdpy(HeaderValue::float_type(-1));

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
            else if( max_cdpy<cdpy ) max_cdpy=cdpy;

            if( iline<min_iline ) min_iline=iline;
            else if( max_iline<iline ) max_iline=iline;

            if( xline<min_xline ) min_xline=xline;
            else if( max_xline<xline ) max_xline=xline;

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
    catch(seismic::SEGYFormatError& err){
        QMessageBox::critical(this, "Scanning SEGY-file", QString("Exception occured:\n")+QString(err.what()));
        destroyReader();
    }

        if( m_reader) m_reader->setInfo(orig_info);

}

void SegyInputDialog::on_pbBrowse_clicked()
{
    QString fn=QFileDialog::getOpenFileName(0, "Open SEGY file");
    if( fn.isNull()) return;        // cancelled
    ui->leFilename->setText(fn);    // filename always synchronized with lineedit
    readInfoFile();
    openReader();                    // openReader uses filename from lineedit
    if(m_reader) updateFixScalcoFromReader();
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
        connect( m_TraceHeaderDialog->navigationWidget(), SIGNAL( currentChanged(size_t)), this, SLOT(navigateToTraceHeader(size_t)));
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

    std::cout<<"READER PARAMS changed!"<<std::endl<<std::flush;
    m_infoDirty=true;

    if( ! m_updateReaderParamsEnabled ) return;

    openReader();

    if( m_TraceHeaderDialog ){
        navigateToTraceHeader( m_TraceHeaderDialog->navigationWidget()->current() );    // reload trace header with adjusted reader params
    }
}


void SegyInputDialog::openReader(){

      if( ui->leFilename->text().isEmpty()) return;

      // close already open reader
      if( m_reader ){
          m_reader.reset();
      }

      try{

          updateInfoFromControls();


          // open a reader
          std::shared_ptr<seismic::SEGYReader> reader(new seismic::SEGYReader(ui->leFilename->text().toStdString(), m_info) );


          // everything ok, keep reader
          m_reader=reader;

       }

      catch( std::exception& err){
          QMessageBox::critical(this, "Open SEGY-file", QString("Exception occured:\n")+QString(err.what()));
           //ui->leFilename->setText("");
      }
      //catch( ... ){
      //    qFatal("EXCEPTION");
      //}

      updateButtons();
}


void SegyInputDialog::destroyReader(){
    m_reader.reset();
    updateButtons();
}

void SegyInputDialog::updateFixScalcoFromReader(){

    try{

        m_reader->seek_trace(0);

        seismic::Header th=m_reader->read_trace_header();

        if( th.find("scalco")!=th.end() ){

            int scalco=th["scalco"].intValue();

            double scalFactor=1;

            if( scalco<0 ){
                scalFactor=-1./scalco;
            }
            else if( scalco>0 ){
                scalFactor=1./scalco;
            }

            ui->leScalco->setText(QString::number(scalFactor));
       }
    }
    catch( std::exception& err){
        QMessageBox::critical(this, "update fixed scalco from reader", QString("Exception occured:\n")+QString(err.what()));
    }

}

void SegyInputDialog::updateButtons(){
      bool on=(m_reader)?true:false;
      ui->btScan->setEnabled(on);
      ui->pbEBCDIC->setEnabled(on);
      ui->pbBinary->setEnabled(on);
      ui->pbScanTraceHeaders->setEnabled(on);
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

    m_BinaryDialog->setData(m_reader->binaryHeader(), m_reader->info().binaryHeaderDef());

    m_BinaryDialog->setWindowTitle( QString( "Binary Header of \"") + ui->leFilename->text() + QString("\""));

    m_BinaryDialog->show();
}

void SegyInputDialog::onTraceHeaderDialogFinished(){
    m_TraceHeaderDialog=nullptr;
}

// n is one based, READER TRACE NUMBERS ARE 0 BASED!
 void SegyInputDialog::navigateToTraceHeader( size_t n ){

     Q_ASSERT( m_reader );

    try{

         n--;

        m_reader->seek_trace(n);

        seismic::Header hdr=m_reader->read_trace_header();

        // repos. stream at beg of header for next read!
        m_reader->seek_trace(n);

        m_TraceHeaderDialog->setData( hdr, m_reader->info().traceHeaderDef());

        m_TraceHeaderDialog->setWindowTitle( QString( "Trace Header of \"") + ui->leFilename->text() + QString("\""));
    }
    catch( seismic::SEGYFormatError& err ){
        QMessageBox::critical(this, "Scanning SEGY-file", QString("Exception occured:\n")+QString(err.what()));
        destroyReader();
    }
}

seismic::SEGYHeaderWordDataType SegyInputDialog::indexToDataType( int idx ){

    seismic::SEGYHeaderWordDataType t;

    switch(idx){
    case 0: t=seismic::SEGYHeaderWordDataType::INT16;break;
    case 1: t=seismic::SEGYHeaderWordDataType::INT32;break;
    case 2: t=seismic::SEGYHeaderWordDataType::IEEE;break;
    default: throw std::runtime_error("illegal segy header word type");
    }

    return t;

}

int SegyInputDialog::DataTypeToIndex( seismic::SEGYHeaderWordDataType t){

    int idx;

    switch(t){
    case seismic::SEGYHeaderWordDataType::INT16: idx=0; break;
    case seismic::SEGYHeaderWordDataType::INT32: idx=1; break;
    case seismic::SEGYHeaderWordDataType::IEEE: idx=2; break;
    default: throw std::runtime_error("illegal segy header word type");
    }

    return idx;
}

std::vector<seismic::SEGYHeaderWordDef> SegyInputDialog::getTraceHeaderDefinition(){

    std::vector<seismic::SEGYHeaderWordDef> thdef = seismic::SEGY_DEFAULT_TRACE_HEADER_DEF;
    for( auto& d : thdef){
        if( d.name == "iline"){
            d.pos=ui->lePosIline->text().toUInt();
            d.dtype=indexToDataType( ui->cbSizeIline->currentIndex());
        }
        else if( d.name == "xline"){
            d.pos=ui->lePosXline->text().toUInt();
            d.dtype=indexToDataType( ui->cbSizeXline->currentIndex());
        }
        else if( d.name == "cdp"){
            d.pos=ui->lePosCDP->text().toUInt();
            d.dtype=indexToDataType( ui->cbSizeCDP->currentIndex());
        }
        else if( d.name == "cdpx"){
            d.pos=ui->lePosCDPX->text().toUInt();
            d.dtype=indexToDataType( ui->cbSizeCDPX->currentIndex());
        }
        else if( d.name == "cdpy"){
            d.pos=ui->lePosCDPY->text().toUInt();
            d.dtype=indexToDataType( ui->cbSizeCDPY->currentIndex());
        }
        else if( d.name == "offset"){
            d.pos=ui->lePosOffset->text().toUInt();
            d.dtype=indexToDataType( ui->cbSizeOffset->currentIndex());
        }

        // check for out of bounds header word pos
        size_t end=d.pos+( ( d.dtype==seismic::SEGYHeaderWordDataType::INT16 ) ? 2 : 4 );
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

    writeInfoFile();

    if( m_reader ) accept();
}

QString SegyInputDialog::infoFileName(){

    if( ui->leFilename->text().isEmpty()) return QString();

    QFileInfo fi( ui->leFilename->text());
    QString fileInfoPath = fi.canonicalPath() + QDir::separator() + fi.baseName() + ".xsi";

    return fileInfoPath;

}

void SegyInputDialog::readInfoFile(){

    QString fileInfoPath = infoFileName();
    if( fileInfoPath.isEmpty()) return;

    QFile file(fileInfoPath);
    if( !file.exists()) return;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Save Segy File Information"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileInfoPath)
                             .arg(file.errorString()));
        return;
    }

    seismic::SEGYInfo info;

    seismic::XSIReader reader(info);
    if (!reader.read(&file)) {
        QMessageBox::warning(this, tr("Load SEGY Info"),
                             tr("Parse error in file %1:\n\n%2")
                             .arg(fileInfoPath)
                             .arg(reader.errorString()));

        return;
    }

    setInfo( info );
    m_infoDirty=false;
}

void SegyInputDialog::writeInfoFile(){

    QString fileInfoPath = infoFileName();

    QFile file(fileInfoPath);



    if( file.exists()){

        if( !m_infoDirty ) return;      // nothing to save

        if( QMessageBox::warning(this, "Save SEGY File Info", QString("File \"%1\" exists. Replace it?").arg(fileInfoPath),
                                 QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) return;
    }

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Save Segy File Information"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileInfoPath)
                             .arg(file.errorString()));
        return;
    }

    seismic::XSIWriter writer(m_info);
    writer.writeFile(&file);

    m_infoDirty=false;
}


void SegyInputDialog::on_pbScanTraceHeaders_clicked()
{
    //std::cout<<"SCANNING..."<<std::endl;
    Q_ASSERT(m_reader);

    seismic::SEGYInfo orig_info=m_reader->info();
    size_t headerSize=m_reader->rawTraceHeader().size();

    std::vector<int16_t> min16(headerSize, std::numeric_limits<int16_t>::max());
    std::vector<int16_t> max16(headerSize, std::numeric_limits<int16_t>::min());
    std::vector<int32_t> min32(headerSize, std::numeric_limits<int32_t>::max());
    std::vector<int32_t> max32(headerSize, std::numeric_limits<int32_t>::min());
    std::vector<float_t> minIEEE(headerSize, std::numeric_limits<float_t>::max());
    std::vector<float_t> maxIEEE(headerSize, std::numeric_limits<float_t>::min());

    try{

        bool swap=ui->cbSwapBytes->isChecked();

        QProgressDialog* pdlg=new QProgressDialog("Scanning Trace Headers", "Cancel", 0, m_reader->trace_count(), this);

        for( int i=0; i<m_reader->trace_count(); i++){   // XXX TEST

            m_reader->seek_trace(i);

            m_reader->read_trace_header();

            std::vector<char> rawHeader=m_reader->rawTraceHeader();

            for(size_t j=0; j+sizeof(int16_t)<headerSize; j++){
                int16_t x;
                seismic::get_from_raw(&x, &rawHeader[j], swap);

                if( x<min16[j]) min16[j]=x;
                if( x>max16[j]) max16[j]=x;
            }

            for(size_t j=0; j+sizeof(int32_t)<headerSize; j++){
                int32_t x;
                seismic::get_from_raw(&x, &rawHeader[j], swap);

                if( x<min32[j]) min32[j]=x;
                if( x>max32[j]) max32[j]=x;
            }

            for(size_t j=0; j+sizeof(float_t)<headerSize; j++){

                float x;
                seismic::get_from_raw(&x, &rawHeader[j], swap);

                if( x<minIEEE[j]) minIEEE[j]=x;
                if( x>maxIEEE[j]) maxIEEE[j]=x;
            }

            pdlg->setValue(i+1);
            qApp->processEvents();
            if( pdlg->wasCanceled()){
                pdlg->setValue(m_reader->trace_count());
                return; // do not update min/max values
            }
        }

        // build model
        QStandardItemModel* model=new QStandardItemModel(headerSize, 5, this);

        QStringList labels;
        labels<<"start pos"<<"min int16"<<"max int16"<<"min int32"<<"max int32"<<"min IEEE"<<"max IEEE";
        model->setHorizontalHeaderLabels(labels);

        for( int i = 0; i< headerSize; i++){

            int column=0;

            model->setItem(i, column++, new QStandardItem(QString::number(i+1))); // positions are 1-based for user
            model->setItem(i, column++, new QStandardItem(QString::number(min16[i])));
            model->setItem(i, column++, new QStandardItem(QString::number(max16[i])));
            model->setItem(i, column++, new QStandardItem(QString::number(min32[i])));
            model->setItem(i, column++, new QStandardItem(QString::number(max32[i])));
            model->setItem(i, column++, new QStandardItem(QString::number(minIEEE[i])));
            model->setItem(i, column++, new QStandardItem(QString::number(maxIEEE[i])));
        }

        if( !m_HeaderScanDialog ){
            m_HeaderScanDialog=new HeaderScanDialog(this);
            m_HeaderScanDialog->setWindowTitle("Trace Header Byte Ranges");
        }
        m_HeaderScanDialog->setModel(model);
        m_HeaderScanDialog->show();
    }
    catch(seismic::SEGYFormatError& err){
        QMessageBox::critical(this, "Scanning SEGY-file", QString("Exception occured:\n")+QString(err.what()));
        destroyReader();
    }
}
