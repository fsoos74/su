#include "computeinterceptgradientprocess.h"

#include<chrono>
#include<iostream>
#include <grid2d.h>
#include<QThread>
#include<QQueue>
#include<QMutex>
#include<QMutexLocker>
#include<QWaitCondition>
#include<QElapsedTimer>


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

ComputeInterceptGradientProcess::ComputeInterceptGradientProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Compute Intercept and Gradient"), project, parent){

}

ProjectProcess::ResultCode ComputeInterceptGradientProcess::init( const QMap<QString, QString>& parameters ){

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

    if( parameters.contains(QString("quality")) ){
        m_qualityName=parameters.value(QString("quality"));
    }

    if( !parameters.contains(QString("reduction-method"))){
        setErrorString("Parameters contain no window reduction method!");
        return ResultCode::Error;
    }
    m_reductionMethod=string2ReductionMethod(parameters.value(QString("reduction-method")));
    m_reductionFunction=reductionFunctionFactory(m_reductionMethod);

    if( !parameters.contains(QString("samples"))){
        setErrorString("Parameters contain no number of samples!");
        return ResultCode::Error;
    }
    m_windowSize=parameters.value(QString("samples")).toInt();

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

    if( parameters.contains("AVA")){
        m_ava=true;
    }
    else{
        m_ava=false;
    }

    if( parameters.contains("depth")){
        m_depth=parameters.value("depth").toDouble();
    }

    if( !parameters.contains(QString("horizon"))){
        setErrorString("Parameters contain no horizon!");
        return ResultCode::Error;
    }
    m_horizonName=parameters.value( QString("horizon") );
    m_horizon=project()->loadGrid( GridType::Horizon, m_horizonName);
    if( !m_horizon ){
        setErrorString("Loading horizon failed!");
        return ResultCode::Error;
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

    //m_slice=std::shared_ptr<Grid2D>( new Grid2D(m_horizon->bounds()));
    Grid2DBounds bounds( m_reader->minInline(), m_reader->minCrossline(),
                           m_reader->maxInline(), m_reader->maxCrossline() );

    m_intercept=std::shared_ptr<Grid2D<double> >( new Grid2D<double> (bounds));
    if( !m_intercept ){
        setErrorString("Allocating intercept grid failed!");
        return ResultCode::Error;
    }

    m_gradient=std::shared_ptr<Grid2D<double> >( new Grid2D<double>(bounds));
    if( !m_gradient ){
        setErrorString("Allocating gradient grid failed!");
        return ResultCode::Error;
    }

    m_quality=std::shared_ptr<Grid2D<double> >( new Grid2D<double>(bounds));
    if( !m_quality ){
        setErrorString("Allocating quality grid failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


// keep this in anonymous namespace for use only in this file!!
namespace{

struct Job{


    int                     firstInline;
    int                     lastInline;
    int                     inlineIncrement;

    Grid2D<double>*         intercept;
    Grid2D<double>*         gradient;
    Grid2D<double>*         quality;
    Grid2D<double>*         horizon;
    GatherBuffer*           buffer;
    int                     supergatherInlineSize;
    int                     supergatherCrosslineSize;
    int                     windowSize;
    ReductionFunction*      reductionFunction;
    bool                    ava;
    double                  depth;

};


class Worker : public QRunnable{

public:

    Worker( const Job& job, volatile bool *stopped ) :
       m_job(job), m_stopped(stopped)
       {
    }

private:

    void run(){

        Grid2DBounds bounds=m_job.intercept->bounds();

        int firstCrossline=m_job.buffer->firstXline();
        int lastCrossline=std::min(firstCrossline+m_job.buffer->xlineCount()-1, m_job.intercept->bounds().j2());

        for( int iline=m_job.firstInline; iline<=m_job.lastInline; iline+=m_job.inlineIncrement ){

            for( int xline=firstCrossline; xline<lastCrossline; xline++){

                seismic::Gather gather=m_job.buffer->gather(iline, xline, m_job.supergatherInlineSize, m_job.supergatherCrosslineSize);

                if( gather.empty() ){
                    continue;
                }
                if( !m_job.horizon->bounds().isInside(iline, xline)) continue;
                double v=(*m_job.horizon)(iline, xline);
                if( v == m_job.horizon->NULL_VALUE ) continue;
                double t=0.001 * v;    // horizon in millis

                QVector<QPointF> curve=buildAmplitudeOffsetCurve(gather, t,
                                                                 m_job.reductionFunction, m_job.windowSize);

                // compute amplitude vs sin^2(theta) where theta is reflection angle
                if( m_job.ava){
                    convertOffsetCurveToAngle(curve, m_job.depth);
                    for( QPointF& p : curve){
                        p.setX(std::pow(std::sin(p.x()),2));
                    }
                }

                double q;
                QPointF interceptAndGradient=linearRegression(curve, &q);

                // linear regression returns nan if all input values are zero
                if( isnan(interceptAndGradient.x()) || isnan(interceptAndGradient.y())){
                    (*m_job.intercept)(iline, xline)=m_job.intercept->NULL_VALUE;
                    (*m_job.gradient)(iline, xline)=m_job.gradient->NULL_VALUE;
                    (*m_job.quality)(iline, xline)=0;
                    continue;
                }

                (*m_job.intercept)(iline, xline)=interceptAndGradient.x();
                (*m_job.gradient)(iline, xline)=interceptAndGradient.y();
                (*m_job.quality)(iline, xline)=q;

            }

        }

    }

private:

    Job                 m_job;
    volatile bool*      m_stopped;
};

}


bool ComputeInterceptGradientProcess::processBuffer_n( GatherBuffer* buffer, int firstIlineToProcess, int lastIlineToProcess ){

    const int nthreads=std::min(QThreadPool::globalInstance()->maxThreadCount(), QThread::idealThreadCount() );
    volatile bool stopped=false;

    for( int t=0; t<nthreads; t++){

        Job job;

        job.firstInline=firstIlineToProcess + t;
        job.lastInline=lastIlineToProcess;
        job.inlineIncrement=nthreads;                   // XXX add/handle overlap for supergathers, remebmber last chunk

        job.buffer=buffer;
        job.horizon=m_horizon.get();
        job.intercept=m_intercept.get();
        job.gradient=m_gradient.get();
        job.quality=m_quality.get();

        job.supergatherInlineSize=m_supergatherInlineSize;
        job.supergatherCrosslineSize=m_supergatherCrosslineSize;

        job.reductionFunction=m_reductionFunction.get();
        job.windowSize=m_windowSize;

        job.ava=m_ava;
        job.depth=m_depth;

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


ProjectProcess::ResultCode ComputeInterceptGradientProcess::run(){

    const int BufferInlineSize=2*QThreadPool::globalInstance()->maxThreadCount() + m_supergatherInlineSize - 1;     // with overlap

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    GatherFilter filter(m_maximumOffset, m_minimumAzimuth, m_maximumAzimuth);

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

    while( reader->current_trace()<reader->trace_count() ){


        qApp->processEvents();

        std::shared_ptr<seismic::Gather> gather=reader->read_gather("cdp");
        if( !gather ) break;

        gather=filter.filter(gather);

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
    std::pair<GridType, QString> interceptTypeAndName=splitFullGridName(m_interceptName);
    if( !project()->addGrid( interceptTypeAndName.first, interceptTypeAndName.second, m_intercept)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_interceptName) );
        return ResultCode::Error;
    }

    std::pair<GridType, QString> gradientTypeAndName=splitFullGridName(m_gradientName);
    if( !project()->addGrid( gradientTypeAndName.first, gradientTypeAndName.second, m_gradient)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_gradientName) );
        return ResultCode::Error;
    }


    if( !m_qualityName.isEmpty()){
        std::pair<GridType, QString> qualityTypeAndName=splitFullGridName(m_qualityName);
        if( !project()->addGrid( qualityTypeAndName.first, qualityTypeAndName.second, m_quality)){
            setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_qualityName) );
            return ResultCode::Error;
        }
    }



    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;

}

