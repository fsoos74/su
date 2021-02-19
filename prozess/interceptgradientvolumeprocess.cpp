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

#include<QDebug>


// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "cdp", "offset", "dt", "ns",
                                       "sx", "sy", "gx", "gy"};

InterceptGradientVolumeProcess::InterceptGradientVolumeProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Intercept and Gradient Volumes"), project, parent){

}

ProjectProcess::ResultCode InterceptGradientVolumeProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_datasetName=getParam( parameters, "dataset");
        m_interceptName=getParam( parameters, "intercept");
        m_gradientName=getParam( parameters, "gradient");
        m_qualityName=getParam( parameters, "quality");
        m_minimumOffset=getParam(parameters, "minimum-offset").toDouble();
        m_maximumOffset=getParam(parameters, "maximum-offset").toDouble();
        m_minimumAzimuth=getParam(parameters, "minimum-azimuth").toDouble();
        m_maximumAzimuth=getParam(parameters, "maximum-azimuth").toDouble();
        m_supergatherMode=static_cast<bool>(getParam(parameters, "supergathers").toInt());
        m_supergatherInlineSize=getParam( parameters, "sg-inline-size").toInt();
        m_supergatherCrosslineSize=getParam( parameters, "sg-crossline-size").toInt();
        m_timeWindow=static_cast<bool>(getParam( parameters, "time-window").toInt());
        m_windowStart=getParam( parameters, "window-start").toInt();
        m_windowEnd=getParam( parameters, "window-end").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    if( !m_supergatherMode){
        m_supergatherInlineSize=m_supergatherCrosslineSize=1;
    }

    m_reader= project()->openSeismicDataset( m_datasetName);
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(m_datasetName) );
        return ResultCode::Error;
    }
    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Prestack ){
        setErrorString("This process was designed for unstacked data!");
        return ResultCode::Error;
    }


    double ft=0;
    double dt=m_reader->segyReader()->dt();
    int nt=m_reader->segyReader()->nt();        // from binary header, need to call this after open

    if( m_timeWindow ){

        if( m_windowStart<0 || m_windowEnd<m_windowStart ){
            setErrorString("Invalid time window!");
            return ResultCode::Error;
        }

        int startSample=std::max( static_cast<int>(std::floor(double(m_windowStart)/(1000*dt))),0);
        int endSample=std::min( static_cast<int>(std::floor(double(m_windowEnd)/(1000*dt))), nt-1);
        m_startTraceSample=startSample;
        nt=endSample- startSample+1;
        ft=0.001*m_windowStart;
    }

    m_bounds=Grid3DBounds( m_reader->minInline(), m_reader->maxInline(),
                           m_reader->minCrossline(), m_reader->maxCrossline(),
                           nt, ft, dt);



#ifdef IO_VOLUMES
    m_interceptWriter=project()->openVolumeWriter(m_interceptName, m_bounds, Domain::Time, VolumeType::AVO);
    if( !m_interceptWriter){
        setErrorString("Open intercept writer failed!");
        return ResultCode::Error;
    }
    m_interceptWriter->setParent(this);

    m_gradientWriter=project()->openVolumeWriter(m_gradientName, m_bounds, Domain::Time, VolumeType::AVO);
    if( !m_gradientWriter){
        setErrorString("Open gradient writer failed!");
        return ResultCode::Error;
    }
    m_gradientWriter->setParent(this);
#else
    m_intercept=std::shared_ptr<Volume>(new Volume(m_bounds));
    m_gradient=std::shared_ptr<Volume>(new Volume(m_bounds));
    m_quality=std::shared_ptr<Volume>(new Volume(m_bounds));      // allways compute quality, just don't save it if not desired
#endif

    std::cout<<"minimum offset="<<m_minimumOffset<<std::endl;
    std::cout<<"maximum offset="<<m_maximumOffset<<std::endl;
    std::cout<<"minimum azimuth="<<m_minimumAzimuth<<std::endl;
    std::cout<<"maximum azimuth="<<m_maximumAzimuth<<std::endl;
    std::cout<<"supergather mode="<<std::boolalpha<<m_supergatherMode<<std::endl;
    std::cout<<"sg inline size="<<m_supergatherInlineSize<<std::endl;
    std::cout<<"sg crossline size="<<m_supergatherCrosslineSize<<std::endl;
    std::cout<<"startMillis="<<m_windowStart<<std::endl;
    std::cout<<"endMillis="<<m_windowEnd<<std::endl;
    std::cout<<"startTraceSample="<<m_startTraceSample<<std::endl;
    std::cout<<"Volumes: ft"<<m_bounds.ft()<<" lt="<<m_bounds.lt()<<" dt="<<m_bounds.dt()<<" nt="<<m_bounds.nt()<<std::endl;
    std::cout<<std::flush;

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
    Volume*          intercept;
    Volume*          gradient;
    Volume*          quality;
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
        int lastCrossline=std::min(firstCrossline+m_job.buffer->xlineCount()-1, m_job.bounds.j2());

        for( int iline=m_job.firstInline; iline<=m_job.lastInline; iline+=m_job.inlineIncrement ){
            for( int xline=firstCrossline; xline<lastCrossline; xline++){

                seismic::Gather gather=m_job.buffer->gather(iline, xline, m_job.supergatherInlineSize, m_job.supergatherCrosslineSize);

                if( gather.empty() ){
                    continue;
                }

                const seismic::Trace& trace=gather.front();

                for( size_t sampleno=0; sampleno<bounds.nt(); sampleno++){

                    QVector<QPointF> curve=buildAmplitudeOffsetCurve(gather,
                                                 m_job.startTraceSample + sampleno);

                    double q;
                    QPointF interceptAndGradient=linearRegression(curve, &q);

                    (*m_job.intercept)(iline, xline, sampleno)=sampleno;//gather[gather.size()-1].header().at("offset").floatValue();
                    (*m_job.gradient)(iline, xline, sampleno)=gather[0].header().at("offset").floatValue();
                    (*m_job.quality)(iline, xline, sampleno)=curve.size();
                    // linear regression returns nan if all input values are zero
                    if( !std::isfinite(interceptAndGradient.x()) || !std::isfinite(interceptAndGradient.y())){
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

/*  WRONG!!! THIS VERSION USES IO_VOLUMES
 * This does not work right
ProjectProcess::ResultCode InterceptGradientVolumeProcess::processBuffer_n( GatherBuffer* buffer, int firstIlineToProcess, int lastIlineToProcess ){

#ifdef IO_VOLUMES
    auto bounds=Grid3DBounds(firstIlineToProcess, lastIlineToProcess,
                             m_bounds.j1(), m_bounds.j2(), m_bounds.nt(), m_bounds.ft(), m_bounds.dt());
    auto intercept=std::make_shared<Volume>(bounds);
    auto gradient=std::make_shared<Volume>(bounds);
    auto quality=std::make_shared<Volume>(bounds);
    if( !intercept || !gradient || !quality){
        setErrorString("Allocating volumes failed!");
        return ResultCode::Error;
    }
#endif

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

#ifdef IO_VOLUMES
        job.intercept=intercept.get();
        job.gradient=gradient.get();
        job.quality=quality.get();
#else
        job.intercept=m_intercept.get();
        job.gradient=m_gradient.get();
        job.quality=m_quality.get();
#endif

        QThreadPool::globalInstance()->start( new Worker(job, &stopped));

    }

    // Wait for worker threads to finish, use polling for now
    const int CHECK_INTERVAL_MILLIS=50;
    while(QThreadPool::globalInstance()->activeThreadCount() > 0){

        QThread::msleep(CHECK_INTERVAL_MILLIS);
        qApp->processEvents();
        if( isCanceled() ) stopped=true;
    }

    if( stopped) return ResultCode::Canceled;

#ifdef IO_VOLUMES
    if( !m_interceptWriter->write(*intercept) ){
        setErrorString("Writing intercept failed!");
        return ResultCode::Error;
    }

    if( !m_gradientWriter->write(*gradient) ){
        setErrorString("Writing gradient failed!");
        return ResultCode::Error;
    }

    if( m_qualityWriter && !m_qualityWriter->write(*quality) ){
        setErrorString("Writing quality failed!");
        return ResultCode::Error;
    }

    intercept.reset();
    gradient.reset();
    quality.reset();
#endif

    return ResultCode::Ok;
}


ProjectProcess::ResultCode InterceptGradientVolumeProcess::run(){

    const int BufferInlineSize=2*QThreadPool::globalInstance()->maxThreadCount() + m_supergatherInlineSize - 1;     // with overlap

    int firstInline=m_reader->minInline();
    int lastInline=m_reader->maxInline();
    int firstCrossline=m_reader->minCrossline();
    int lastCrossline=m_reader->maxCrossline();
    int inlinesToProcess=lastInline-firstInline+1;
    int inlinesProcessed=0;

    emit currentTask("Processing cdps...");
    emit started(m_reader->sizeTraces());
    emit progress(0);
    qApp->processEvents();

    ssize_t lastPercent=0;
    m_reader->seekTrace(0);
    GatherBuffer buffer( m_reader->minInline(), m_reader->minCrossline(), BufferInlineSize, m_reader->maxCrossline()-m_reader->minCrossline()+1);
    int lastFullDataIline=buffer.lastIline()-m_supergatherInlineSize+1;
    GatherFilter filter(m_minimumOffset, m_maximumOffset, m_minimumAzimuth, m_maximumAzimuth);


    while( m_reader->good() ){

        int percent=100*m_reader->tellTrace()/m_reader->sizeTraces();
        if(percent!=lastPercent){
            lastPercent=percent;
            emit progress(m_reader->tellTrace());
            qApp->processEvents();
        }

        auto gather=m_reader->readGather("cdp");
        if( !gather ) break;

        // extract this before filter because filter could leave empty gather
        const seismic::Header& header=gather->front().header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        gather=filter.filter(gather);

        // buffer full? now process
        if( iline>lastFullDataIline){

            int firstIlineToProcess=buffer.firstIline();
            int lastIlineToProcess=firstIlineToProcess+buffer.ilineCount()-m_supergatherInlineSize;

            auto res=processBuffer_n( &buffer, firstIlineToProcess, lastIlineToProcess  );
            if( res!=ResultCode::Ok) return res;

            // copy unfinished inlines to beginning of buffer
            buffer.advanceTo(lastFullDataIline+1);
            lastFullDataIline=buffer.lastIline()-m_supergatherInlineSize+1;
        }

        buffer(iline, xline)=gather;
    }

    // remember last chunk
    int firstIlineToProcess=buffer.firstIline();
    int lastIlineToProcess=std::min( buffer.firstIline()+buffer.ilineCount()-1, lastInline);
    auto res= processBuffer_n( &buffer, firstIlineToProcess, lastIlineToProcess );
    if( res!=ResultCode::Ok) return res;

    emit progress(m_reader->sizeTraces());
    qApp->processEvents();

#ifdef IO_VOLUMES
    m_interceptWriter->flush();
    m_interceptWriter->close();
    m_gradientWriter->flush();
    m_gradientWriter->close();
    if( !project()->addVolume( m_interceptName, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_interceptName) );
        return ResultCode::Error;
    }

    if( !project()->addVolume( m_gradientName, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_gradientName) );
        return ResultCode::Error;
    }

    if( m_qualityWriter ){

        m_qualityWriter->flush();
        m_qualityWriter->close();
        if( !project()->addVolume( m_qualityName, params() )){
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_qualityName) );
            return ResultCode::Error;
        }

    }

#else

    // store result volumes
    if( !project()->addVolume( m_interceptName, m_intercept, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_interceptName) );
        return ResultCode::Error;
    }

    if( !project()->addVolume( m_gradientName, m_gradient, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_gradientName) );
        return ResultCode::Error;
    }

    if( !m_qualityName.isEmpty() ){    // only save quality if name is specified

        if( !project()->addVolume( m_qualityName, m_quality, params() )){
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_qualityName) );
            return ResultCode::Error;
        }

    }
#endif

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;

}
*/




// original
// this version works right, compared results with previously accepted results
ProjectProcess::ResultCode InterceptGradientVolumeProcess::processBuffer_n( GatherBuffer* buffer, int firstIlineToProcess, int lastIlineToProcess ){

#ifdef IO_VOLUMES
    auto bounds=Grid3DBounds(firstIlineToProcess, lastIlineToProcess,
                             m_bounds.j1(), m_bounds.j2(), m_bounds.nt(), m_bounds.ft(), m_bounds.dt());
    auto intercept=std::make_shared<Volume>(bounds);
    auto gradient=std::make_shared<Volume>(bounds);
    auto quality=std::make_shared<Volume>(bounds);
    if( !intercept || !gradient || !quality){
        setErrorString("Allocating volumes failed!");
        return ResultCode::Error;
    }
#endif

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

#ifdef IO_VOLUMES
        job.intercept=intercept.get();
        job.gradient=gradient.get();
        job.quality=quality.get();
#else
        job.intercept=m_intercept.get();
        job.gradient=m_gradient.get();
        job.quality=m_quality.get();
#endif
        QThreadPool::globalInstance()->start( new Worker(job, &stopped));
    }

    // Wait for worker threads to finish, use polling for now
    const int CHECK_INTERVAL_MILLIS=50;
    while(QThreadPool::globalInstance()->activeThreadCount() > 0){

        QThread::msleep(CHECK_INTERVAL_MILLIS);
        qApp->processEvents();
        if( isCanceled() ) stopped=true;

    }

    if( stopped){
        return ResultCode::Canceled;
    }

#ifdef IO_VOLUMES
    if( !m_interceptWriter->write(*intercept) ){
        setErrorString("Writing intercept failed!");
        return ResultCode::Error;
    }

    if( !m_gradientWriter->write(*gradient) ){
        setErrorString("Writing gradient failed!");
        return ResultCode::Error;
    }

    if( m_qualityWriter && !m_qualityWriter->write(*quality) ){
        setErrorString("Writing quality failed!");
        return ResultCode::Error;
    }

    intercept.reset();
    gradient.reset();
    quality.reset();
#endif

    return ResultCode::Ok;
}

ProjectProcess::ResultCode InterceptGradientVolumeProcess::run(){

    const int BufferInlineSize=2*QThreadPool::globalInstance()->maxThreadCount() + m_supergatherInlineSize - 1;     // with overlap

    int firstInline=m_reader->minInline();
    int lastInline=m_reader->maxInline();
    int firstCrossline=m_reader->minCrossline();
    int lastCrossline=m_reader->maxCrossline();
    int inlinesToProcess=lastInline-firstInline+1;
    int inlinesProcessed=0;

    emit currentTask("Processing cdps...");
    emit started(m_reader->sizeTraces());
    emit progress(0);
    qApp->processEvents();

    ssize_t lastPercent=0;
    m_reader->seekTrace(0);
    GatherBuffer buffer( m_reader->minInline(), m_reader->minCrossline(), BufferInlineSize, m_reader->maxCrossline()-m_reader->minCrossline()+1);
    int lastFullDataIline=buffer.lastIline()-m_supergatherInlineSize+1;
    GatherFilter filter(m_minimumOffset, m_maximumOffset, m_minimumAzimuth, m_maximumAzimuth);

    while( m_reader->good() ){

        int percent=100*m_reader->tellTrace()/m_reader->sizeTraces();
        if(percent!=lastPercent){
            lastPercent=percent;
            emit progress(m_reader->tellTrace());
            qApp->processEvents();
        }

        auto gather=m_reader->readGather("cdp");
        if( !gather ) break;

        // extract this before filter because filter could leave empty gather
        const seismic::Header& header=gather->front().header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        gather=filter.filter(gather);

        // buffer full? now process
        if( iline>lastFullDataIline){

            int firstIlineToProcess=buffer.firstIline();
            int lastIlineToProcess=firstIlineToProcess+buffer.ilineCount()-m_supergatherInlineSize;

            auto res= processBuffer_n( &buffer, firstIlineToProcess, lastIlineToProcess  );
            if( res!=ResultCode::Ok) return res;

            // copy unfinished inlines to beginning of buffer
            buffer.advanceTo(lastFullDataIline+1);
            lastFullDataIline=buffer.lastIline()-m_supergatherInlineSize+1;
        }

        buffer(iline, xline)=gather;
    }

    // remember last chunk
    int firstIlineToProcess=buffer.firstIline();
    int lastIlineToProcess=std::min( buffer.firstIline()+buffer.ilineCount()-1, lastInline);
    auto res=processBuffer_n( &buffer, firstIlineToProcess, lastIlineToProcess );
    if( res!=ResultCode::Ok) return res;

    emit progress(m_reader->sizeTraces());
    qApp->processEvents();

#ifdef IO_VOLUMES
    m_interceptWriter->flush();
    m_interceptWriter->close();
    m_gradientWriter->flush();
    m_gradientWriter->close();
    if( !project()->addVolume( m_interceptName, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_interceptName) );
        return ResultCode::Error;
    }

    if( !project()->addVolume( m_gradientName, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_gradientName) );
        return ResultCode::Error;
    }

    if( m_qualityWriter ){

        m_qualityWriter->flush();
        m_qualityWriter->close();
        if( !project()->addVolume( m_qualityName, params() )){
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_qualityName) );
            return ResultCode::Error;
        }

    }

#else
    // store result volumes
    if( !project()->addVolume( m_interceptName, m_intercept, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_interceptName) );
        return ResultCode::Error;
    }

    if( !project()->addVolume( m_gradientName, m_gradient, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_gradientName) );
        return ResultCode::Error;
    }

    if( !m_qualityName.isEmpty() ){    // only save quality if name is specified

        if( !project()->addVolume( m_qualityName, m_quality, params() )){
            setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_qualityName) );
            return ResultCode::Error;
        }

    }
#endif

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;

}



