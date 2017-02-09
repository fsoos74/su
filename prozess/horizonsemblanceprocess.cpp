#include "horizonsemblanceprocess.h"

#include<grid2d.h>
#include<seismicdatasetreader.h>
#include<memory>
#include<iostream>

#include<QRunnable>
#include<QThreadPool>

#include "utilities.h"
#include <semblance.h>

// necessary to speed up slow segy input
const QStringList REQUIRED_HEADER_WORDS{ "iline", "xline", "cdp", "offset", "dt", "ns" };


double semblance( const seismic::Gather& g, size_t halfSamples, double tt);

HorizonSemblanceProcess::HorizonSemblanceProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Horizon Semblance"), project, parent){

}

ProjectProcess::ResultCode HorizonSemblanceProcess::init( const QMap<QString, QString>& parameters ){

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

    if( !parameters.contains(QString("grid"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_sliceName=parameters.value(QString("grid"));

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
    if( m_reader->info().mode()!=SeismicDatasetInfo::Mode::Poststack ){
        setErrorString("This process was designed for stacked data!");
        return ResultCode::Error;
    }

    //std::cout<<"Dataset: "<<dataset.toStdString()<<std::endl;
    //std::cout<<"iline range: "<< m_reader->minInline()<<" - "<< m_reader->maxInline()<<std::endl;
    //std::cout<<"xline range: "<< m_reader->minCrossline()<<" - "<< m_reader->maxCrossline()<<std::endl;

    m_slice=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(m_horizon->bounds()));
    if( !m_slice ){
        setErrorString("Allocating slice failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

struct Job{

    Grid2D<float>*                 grid;               // this is the result
    const Grid2D<seismic::Trace*>*  traceBuffer;
    const Grid2D<float>*           horizon;

    int                             halfIlines;
    int                             halfXlines;
    int                             halfSamples;

    int                             iline;
    int                             firstXLine;
    int                             xlineIncr;
};


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

            Grid2DBounds bounds=job.grid->bounds();

            int il=job.iline;

            for( int xl=job.firstXLine; xl<=bounds.j2(); xl+=job.xlineIncr){

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

                //if( !job.horizon->bounds().isInside(il, xl)) continue;  // valueAt does the job
                double v=job.horizon->valueAt(il, xl);
                if( v == job.horizon->NULL_VALUE ) continue;
                double t=0.001 * v;    // horizon in millis
                (*job.grid)(il, xl)=semblance(gather, job.halfSamples, t);
            }
        }

    }

private:

    volatile bool* m_stopped;
};




ProjectProcess::ResultCode HorizonSemblanceProcess::run(){

    std::shared_ptr<seismic::SEGYReader> reader=m_reader->segyReader();
    if( !reader){
        setErrorString("Invalid segyreader!");
        return ResultCode::Error;
    }

    // workaround: convert only required trace header words on input, seems to be necessary on windows because otherwise too slow!!!
    setRequiredHeaderwords(*reader, REQUIRED_HEADER_WORDS);

    Grid2DBounds bounds=m_slice->bounds();
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

        int firstXLine=m_horizon->bounds().j1();

        for( int i=0; i<nthreads; i++){

            Job job;
            job.traceBuffer=&traceBuffer;
            job.horizon=m_horizon.get();
            job.grid=m_slice.get();
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

    std::pair<GridType, QString> sliceTypeAndName = splitFullGridName( m_sliceName );
    if( !project()->addGrid( sliceTypeAndName.first, sliceTypeAndName.second, m_slice)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_sliceName) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

/*

double semblance( const seismic::Gather& g, size_t halfSamples, double tt){

    const size_t& hw=halfSamples;

    size_t N=g.size();

    if( N<2 ) return 1;

    std::vector<double> st;
    st.assign( 2*hw + 1 , 0);
    double fnt2=0;

    for( size_t n=0; n<N; n++ ){
        const seismic::Trace& trc=g[n];
        if( trc.size()<1 )continue;
        int iline=trc.header().at("iline").intValue();
        int xline=trc.header().at("xline").intValue();
        //double tt=0.001 * (*m_horizon)( iline, xline );
        size_t tn(trc.time_to_index( tt ));
        size_t t1((tn>=hw)?tn - hw : 0);
        size_t t2((tn+hw<trc.size()) ? tn+hw : trc.size()-1);
//
//    if( tn<hw || tn + hw>=trc.size() ) continue;
//    size_t t1=tn-hw;
//    size_t t2=tn+hw;
//
        for( size_t t=t1; t<=t2; t++){
            const double& ft=trc[t];
            st[t-t1]+=ft;
            fnt2+=ft*ft;
        }
    }

    double s2=0;
    for( auto s : st ){
        s2+=s*s;
    }

    if( !fnt2 ) return -1;

    return ( s2 - fnt2)/( (N-1)*fnt2);
}
*/
