#include "logspectrumviewer.h"
#include "ui_logspectrumviewer.h"

#include<QInputDialog>
#include<QMessageBox>
#include<fft.h>
#include<spectrum.h>
#include<iostream>

Spectrum computeSpectrum( const Log& log){

    // find fft window len, must be power of 2
    size_t fftlen = 1;
    while( fftlen < log.nz() ) fftlen *= 2;

    // fill input buffer for fft
    std::vector<float> ibuf(fftlen, 0);
    std::copy( log.cbegin(), log.cend(), ibuf.begin() );
    //std::replace( ibuf.begin(), ibuf.end(), log.NULL_VALUE, 0.);     //
    //std::remove_if( ibuf.begin(), ibuf.end(), [&log](double x){return x==log.NULL_VALUE;});
    // ADD additional code here for window len not power of 2 (e.g. padding)

    // fft
    auto obuf=fft(ibuf);

    // compute frequency step
    auto df = 1. / ( fftlen * log.dz() );

    std::cout<<"dz="<<log.dz()<<" df="<<df<<std::endl<<std::flush;

    return Spectrum( 0, df, obuf.begin(), obuf.end() );
}

LogSpectrumViewer::LogSpectrumViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LogSpectrumViewer)
{
    ui->setupUi(this);

    ui->spectrumView->setCursorMode(AxisView::CURSORMODE::BOTH);
    ui->spectrumView->setZoomMode(AxisView::ZOOMMODE::BOTH);
    ui->spectrumView->setXMesh(true);
    ui->spectrumView->setZMesh(true);
    ui->spectrumView->xAxis()->setName(tr("Frequency"));
    ui->spectrumView->zAxis()->setName(tr("Amplitude"));
}

LogSpectrumViewer::~LogSpectrumViewer()
{
    delete ui;
}


void LogSpectrumViewer::setProject( AVOProject* p){

    if( p==m_project ) return;

    m_project=p;
}


void LogSpectrumViewer::setUWI(const QString & s){

    if( !m_project->existsWell(s)) return;

    m_uwi=s;
    auto info=m_project->getWellInfo(s);
    setWindowTitle(QString("%1 - %2").arg(s,info.name()));
}


void LogSpectrumViewer::setLog(std::shared_ptr<Log> log){

    if( !log ) return;

    auto s = std::make_shared<Spectrum>(computeSpectrum(*log));

    ui->spectrumView->setSpectrum(s);

    setWindowTitle( QString("Spectrum - %1 - %2").arg(m_uwi, QString(log->name().c_str())));

}



void LogSpectrumViewer::openLog(){

    bool ok=false;
    QString name = QInputDialog::getItem(this, tr("Open Log"), tr("Select Log:"), m_project->logList(m_uwi), 0, false, &ok );

    if( name.isEmpty() || !ok ) return;

    openLog(name);
}


void LogSpectrumViewer::openLog(QString name){

    auto log=m_project->loadLog(m_uwi, name);
        if( !log ){
        QMessageBox::critical(this, tr("Open Log"), QString("Loading log \"%1 - %2\" failed!").arg(m_uwi).arg(name));
        return;
    }

    setLog(log);

}
