#include "semblancevolumeprocess.h"

#include<grid2d.h>
#include<seismicdatasetreader.h>
#include<memory>
#include<iostream>

#include<QRunnable>
#include<QThreadPool>

#include "utilities.h"
#include <semblance.h>

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "dt", "ns" };



SemblanceVolumeProcess::SemblanceVolumeProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Semblance Volume"), project, parent){

}

ProjectProcess::ResultCode SemblanceVolumeProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("half-ilines"))){
        setErrorString("Parameters contain no half-ilines");
        return ResultCode::Error;
    }
    m_halfIlines=parameters.value(QString("half-ilines") ).toUInt();

    if( !parameters.contains(QString("half-xlines"))){
        setErrorString("Parameters contain no half-xlines");
        return ResultCode::Error;
    }
    m_halfXlines=parameters.value(QString("half-xlines") ).toUInt();

    if( !parameters.contains(QString("half-samples"))){
        setErrorString("Parameters contain no half-samples");
        return ResultCode::Error;
    }
    m_halfSamples=parameters.value(QString("half-samples") ).toUInt();

    if( !parameters.contains(QString("dataset"))){
        setErrorString("Parameters contain no dataset!");
        return ResultCode::Error;
    }
    QString dataset=parameters.value(QString("dataset"));

    if( !parameters.contains(QString("volume"))){
        setErrorString("Parameters contain no output volume!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value(QString("volume"));


    m_reader= project()->openSeismicDataset( dataset);
    if( !m_reader){
        setErrorString(QString("Open dataset \"%1\" failed!").arg(dataset) );
        return ResultCode::Error;
    }
    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Poststack ){
        setErrorString("This process was designed for stacked data!");
        return ResultCode::Error;
    }


    m_bounds=Grid3DBounds( m_reader->minInline(), m_reader->maxInline(),
                           m_reader->minCrossline(), m_reader->maxCrossline(),
                           m_reader->segyReader()->nt(), 0, m_reader->segyReader()->dt());

    m_volume=std::shared_ptr<Grid3D<float>>(new Grid3D<float>(m_bounds));

    if( !m_volume ){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

struct Job{

    Grid3D<float>*                  volume;               // this is the result
    const Grid2D<seismic::Trace*>*  traceBuffer;

    int                             halfIlines;
    int                             halfXlines;
    int                             halfSamples;

    int                             iline;
    int                             firstXLine;
    int                             xlineIncr;
};

namespace{


QQueue<Job>     jobs;
QMutex          jobMutex;
QMutex          jobFinishMutex;
QWaitCondition  jobFinishWait;

class Worker : public QRunnable{

public:

    Worker( volatile bool *stopped ) :
       m_stopped(stopped)
       {

    }


protected:

    void run(){

        forever{

            bool finished=false;
            Job job;

            jobMutex.lock();
            if( jobs.empty()){
                finished=true;
            }
            else{
                job=jobs.dequeue();
            }
            jobMutex.unlock();

            if( finished ) return;

            Grid3DBounds bounds=job.volume->bounds();

            int il=job.iline;

            for( int xl=job.firstXLine; xl<=bounds.crossline2(); xl+=job.xlineIncr){

                if(*m_stopped) return;

                seismic::Gather gather;
                for( int ii=-job.halfIlines; ii<=job.halfIlines; ii++){
                    int iline=il+ii;

                    for(int jj=-job.halfXlines; jj<=job.halfXlines; jj++){
                        int xline=xl+jj;

                        if(bounds.isInside(iline, xline) ){

                            seismic::Trace* t=(*job.traceBuffer)(iline, xline);
                            if( t ){
                                gather.push_back( *t );
                            }
                        }
                    }
                }

                if( gather.empty() ) continue;

                int nt=bounds.sampleCount();
                for( int i=0; i<nt; i++){
                    double t=bounds.sampleToTime(i);
                    (*job.volume)(il, xl, i)=semblance(gather, job.halfSamples, t);
                }
            }
        }

    }

private:

    volatile bool* m_stopped;
};


}

ProjectProcess::ResultCode SemblanceVolumeProcess::run(){

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    Grid2DBounds bounds(m_bounds.inline1(), m_bounds.crossline1(), m_bounds.inline2(), m_bounds.crossline2() );
    Grid2D<seismic::Trace*> traceBuffer(bounds);


    emit currentTask(QString("Initializing..."));
    emit started( reader->trace_count() );
    qApp->processEvents();

    reader->seek_trace(0);
    size_t n=0;
    while( reader->current_trace() < reader->trace_count() ){

        seismic::Trace trace=reader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();
        if( bounds.isInside(iline, xline)){
            traceBuffer(iline, xline)=new seismic::Trace( trace );
            n++;
        }
        emit progress(reader->current_trace());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }



    const int nthreads=QThreadPool::globalInstance()->maxThreadCount();
    volatile bool stopped=false;

    for( int il=bounds.i1(); il<=bounds.i2(); il++){

        int firstXLine=bounds.j1();

        for( int i=0; i<nthreads; i++){

            Job job;
            job.traceBuffer=&traceBuffer;
            job.volume=m_volume.get();
            job.halfIlines=m_halfIlines;
            job.halfXlines=m_halfXlines;
            job.halfSamples=m_halfSamples;
            job.iline=il;
            job.firstXLine=firstXLine;
            job.xlineIncr=nthreads;

            jobs.enqueue(job);

            firstXLine++;
        }

    }

    int total=jobs.size();
    emit currentTask(QString("Running..."));
    emit started( total );
    qApp->processEvents();

    for( int i=0; i<nthreads; i++){

        QThreadPool::globalInstance()->start( new Worker(&stopped));
    }

    std::cout<<"running: "<<QThreadPool::globalInstance()->activeThreadCount()<<std::endl;

    while(QThreadPool::globalInstance()->activeThreadCount() > 0){

        jobMutex.lock();
        int n=jobs.size();

        emit progress(total - n);
        qApp->processEvents();
        if( isCanceled()){
            stopped=true;
        }

        jobMutex.unlock();


    }

    emit finished();

    if( stopped) return ResultCode::Canceled;

    if( !project()->addVolume( m_volumeName, m_volume)){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


