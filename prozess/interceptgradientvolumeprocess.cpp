#include "interceptgradientvolumeprocess.h"

#include<chrono>
#include<iostream>
#include <grid3d.h>
#include<QThread>
#include<QElapsedTimer>
#include<volumewriter.h>
#include<fstream>
#include<iomanip>

#include<segyinfo.h>
#include<segy_header_def.h>

#include "amplitudecurves.h"
#include "gatherfilter.h"
#include "linearregression.h"
#include "gatherbuffer.h"

#include "utilities.h"


#include <QStringList>


// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "cdp", "offset", "dt", "ns",
                                       "sx", "sy", "gx", "gy"};

InterceptGradientVolumeProcess::InterceptGradientVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Intercept and Gradient Volumes"), project, parent){

}

ProjectProcess::ResultCode InterceptGradientVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("dataset"))){
        setErrorString("Parameters contain no dataset!");
        return ResultCode::Error;
    }
    QString dataset=parameters.value(QString("dataset"));

    if( !parameters.contains(QString("intercept"))){
        setErrorString("Parameters contain no output intercept grid name!");
        return ResultCode::Error;
    }
    m_interceptName=parameters.value(QString("intercept"));

    if( !parameters.contains(QString("gradient"))){
        setErrorString("Parameters contain no output gradient grid name!");
        return ResultCode::Error;
    }
    m_gradientName=parameters.value(QString("gradient"));

    if( parameters.contains(QString("quality"))){
        m_qualityName=parameters.value(QString("quality"));
    }

    if( !parameters.contains(QString("maximum-offset"))){
        setErrorString("Parameters contain no maximum offset!");
        return ResultCode::Error;
    }
    m_maximumOffset=parameters.value(QString("maximum-offset")).toDouble();

    if(!parameters.contains("minimum-azimuth")){
        setErrorString("Parameters contain no minimum-azimuth");
        return ResultCode::Error;
    }
    m_minimumAzimuth=parameters.value("minimum-azimuth").toDouble();

    if(!parameters.contains("maximum-azimuth")){
        setErrorString("Parameters contain no maximum-azimuth");
        return ResultCode::Error;
    }
    m_maximumAzimuth=parameters.value("maximum-azimuth").toDouble();


    if( parameters.contains(QString("sg-inline-size")) && parameters.contains(QString("sg-crossline-size"))){
        m_supergatherMode=true;
        m_supergatherInlineSize=parameters.value(QString("sg-inline-size")).toInt();
        m_supergatherCrosslineSize=parameters.value(QString("sg-crossline-size")).toInt();
    }
    else{
        m_supergatherMode=false;
    }

    m_reader= project()->openSeismicDataset( dataset);
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(dataset) );
        return ResultCode::Error;
    }
    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Prestack ){
        setErrorString("This process was designed for unstacked data!");
        return ResultCode::Error;
    }


    double dt=m_reader->segyReader()->dt();
    int nt=m_reader->segyReader()->nt();        // from binary header, need to call this after open
    int startMillis=0;
    int endMillis=0;
    if( parameters.contains("window-mode")){

        startMillis=parameters.value("window-start").toInt();
        endMillis=parameters.value("window-end").toInt();

        if( startMillis>endMillis) std::swap(startMillis, endMillis);

        if( startMillis<0 ) startMillis=0;
        int startSample=std::max( static_cast<int>(std::floor(double(startMillis)/(1000*dt))),0);
        int endSample=std::min( static_cast<int>(std::floor(double(endMillis)/(1000*dt))), nt-1);
        m_startTraceSample=startSample;
        nt=endSample- startSample+1;
    }

    double ft=0.001*startMillis;
    m_bounds=Grid3DBounds( m_reader->minInline(), m_reader->maxInline(),
                           m_reader->minCrossline(), m_reader->maxCrossline(),
                           nt, ft, dt);
/*
std::cout<<"startMillis="<<startMillis<<std::endl;
std::cout<<"endMillis="<<endMillis<<std::endl;
std::cout<<"startTraceSample="<<m_startTraceSample<<std::endl;
std::cout<<"Volumes: ft"<<m_bounds.ft()<<" lt="<<m_bounds.lt()<<" dt="<<m_bounds.dt()<<" nt="<<m_bounds.sampleCount()<<std::endl;
*/
    m_intercept=std::shared_ptr<Grid3D<float>>(new Grid3D<float>(m_bounds));
    m_gradient=std::shared_ptr<Grid3D<float>>(new Grid3D<float>(m_bounds));
    m_quality=std::shared_ptr<Grid3D<float>>(new Grid3D<float>(m_bounds));      // allways compute quality, just don't save it if not desired

    return ResultCode::Ok;
}


// keep this in anonymous namespace for use only in this file!!
namespace{

struct Job{


    int                     firstInline;
    int                     lastInline;
    int                     inlineIncrement;

    GatherBuffer*           buffer;
    int                     supergatherInlineSize;
    int                     supergatherCrosslineSize;
    int                     startTraceSample;


    Grid3DBounds            bounds;
    Grid3D<float>*          intercept;
    Grid3D<float>*          gradient;
    Grid3D<float>*          quality;
};


class Worker : public QRunnable{

public:

    Worker( const Job& job, volatile bool *stopped ) :
       m_job(job), m_stopped(stopped)
       {
    }

private:

    void run(){

        Grid3DBounds bounds=m_job.bounds;

        int firstCrossline=m_job.buffer->firstXline();
        int lastCrossline=std::min(firstCrossline+m_job.buffer->xlineCount()-1, m_job.bounds.crossline2());

        for( int iline=m_job.firstInline; iline<=m_job.lastInline; iline+=m_job.inlineIncrement ){

            for( int xline=firstCrossline; xline<lastCrossline; xline++){

                seismic::Gather gather=m_job.buffer->gather(iline, xline, m_job.supergatherInlineSize, m_job.supergatherCrosslineSize);

                if( gather.empty() ){
                    continue;
                }

                const seismic::Trace& trace=gather.front();

                for( size_t sampleno=0; sampleno<bounds.sampleCount(); sampleno++){

                    QVector<QPointF> curve=buildAmplitudeOffsetCurve(gather,
                                                 m_job.startTraceSample + sampleno);

                    double q;
                    QPointF interceptAndGradient=linearRegression(curve, &q);

                    // linear regression returns nan if all input values are zero
                    if( std::isnan(interceptAndGradient.x()) || std::isnan(interceptAndGradient.y())){
                        (*m_job.intercept)(iline, xline,sampleno)=m_job.intercept->NULL_VALUE;
                        (*m_job.gradient)(iline, xline,sampleno)=m_job.gradient->NULL_VALUE;
                        (*m_job.quality)(iline, xline,sampleno)=m_job.quality->NULL_VALUE;
                        continue;
                    }

                    (*m_job.intercept)(iline, xline, sampleno)=interceptAndGradient.x();
                    (*m_job.gradient)(iline, xline, sampleno)=interceptAndGradient.y();
                    (*m_job.quality)(iline, xline, sampleno)=q;
                }

            }

        }

    }

private:

    Job                 m_job;
    volatile bool*      m_stopped;
};

}


bool InterceptGradientVolumeProcess::processBuffer_n( GatherBuffer* buffer, int firstIlineToProcess, int lastIlineToProcess ){

    const int nthreads=std::min(QThreadPool::globalInstance()->maxThreadCount(), QThread::idealThreadCount() );
    volatile bool stopped=false;

    for( int t=0; t<nthreads; t++){

        Job job;

        job.firstInline=firstIlineToProcess + t;
        job.lastInline=lastIlineToProcess;
        job.inlineIncrement=nthreads;                   // XXX add/handle overlap for supergathers, remebmber last chunk

        job.buffer=buffer;
        job.bounds=m_bounds;

        job.supergatherInlineSize=m_supergatherInlineSize;
        job.supergatherCrosslineSize=m_supergatherCrosslineSize;
        job.startTraceSample=m_startTraceSample;

        job.intercept=m_intercept.get();
        job.gradient=m_gradient.get();
        job.quality=m_quality.get();

        QThreadPool::globalInstance()->start( new Worker(job, &stopped));

    }

    // Wait for worker threads to finish, use polling for now
    const int CHECK_INTERVAL_MILLIS=50;
    while(QThreadPool::globalInstance()->activeThreadCount() > 0){

        QThread::msleep(CHECK_INTERVAL_MILLIS);
        qApp->processEvents();
        if( isCanceled() ) stopped=true;

    }

    return !stopped;


}


ProjectProcess::ResultCode InterceptGradientVolumeProcess::run(){

    const int BufferInlineSize=2*QThreadPool::globalInstance()->maxThreadCount() + m_supergatherInlineSize - 1;     // with overlap

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    int firstInline=m_reader->minInline();
    int lastInline=m_reader->maxInline();
    int firstCrossline=m_reader->minCrossline();
    int lastCrossline=m_reader->maxCrossline();
    int inlinesToProcess=lastInline-firstInline+1;
    int inlinesProcessed=0;

    emit currentTask("Processing inlines...");
    emit started(100);  // percent
    emit progress(0);
    qApp->processEvents();

    reader->seek_trace(0);
    GatherBuffer buffer( m_reader->minInline(), m_reader->minCrossline(), BufferInlineSize, m_reader->maxCrossline()-m_reader->minCrossline()+1);
    int lastFullDataIline=buffer.lastIline()-m_supergatherInlineSize+1;
    GatherFilter filter(m_maximumOffset, m_minimumAzimuth, m_maximumAzimuth);

    while( reader->current_trace()<reader->trace_count() ){


        qApp->processEvents();

        std::shared_ptr<seismic::Gather> gather=reader->read_gather("cdp");
        if( !gather ) break;

        // extract this before filter because filter could leave empty gather
        const seismic::Header& header=gather->front().header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        // buffer full, now process
        if( iline>lastFullDataIline){

            int firstIlineToProcess=buffer.firstIline();
            int lastIlineToProcess=firstIlineToProcess+buffer.ilineCount()-m_supergatherInlineSize;

            if( ! processBuffer_n( &buffer, firstIlineToProcess, lastIlineToProcess  ) ){     // XXX
                return ResultCode::Canceled;
            }

            // copy unfinished inlines to beginning of buffer
            buffer.advanceTo(lastFullDataIline+1);
            int percentDone=100*(lastFullDataIline-firstInline+1)/(lastInline-firstInline+1);
            lastFullDataIline=buffer.lastIline()-m_supergatherInlineSize+1;
             emit progress(percentDone);
        }

        buffer(iline, xline)=gather;
    }

    // remember last chunk
    int firstIlineToProcess=buffer.firstIline();
    int lastIlineToProcess=std::min( buffer.firstIline()+buffer.ilineCount()-1, lastInline);
    if( ! processBuffer_n( &buffer, firstIlineToProcess, lastIlineToProcess ) ){
        return ResultCode::Canceled;
    }
    int percentDone=100;
    emit progress(percentDone);
    qApp->processEvents();


    // store result grids
    if( !project()->addVolume( m_interceptName, m_intercept)){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_interceptName) );
        return ResultCode::Error;
    }

    if( !project()->addVolume( m_gradientName, m_gradient)){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_gradientName) );
        return ResultCode::Error;
    }

    if( !m_qualityName.isEmpty() ){    // only save quality if name is specified

        if( !project()->addVolume( m_qualityName, m_quality)){
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_qualityName) );
            return ResultCode::Error;
        }

    }

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;

}

