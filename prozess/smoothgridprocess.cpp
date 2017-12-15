#include "smoothgridprocess.h"

#include <avoproject.h>
using namespace std::placeholders;

SmoothGridProcess::SmoothGridProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Smooth Grid"), project, parent){

}

ProjectProcess::ResultCode SmoothGridProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{

        QString gtype=getParam(parameters, "grid-type");
        m_gridType = toGridType(gtype);
        m_outputName=getParam(parameters,"output-name");
        m_inputName=getParam(parameters,"input-name");
        m_halfIlines=getParam(parameters,"half-ilines" ).toInt();
        m_halfXlines=getParam(parameters,"half-xlines" ).toInt();
        int mi=getParam(parameters, "method").toInt();
        m_method=static_cast<Method>(mi);
        int rt=getParam(parameters, "retain-values").toInt();
        m_retainValues=static_cast<bool>(rt);
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }


    switch( m_method){
    case Method::Mean: m_filter=std::bind(&SmoothGridProcess::mean, this, _1, _2); break;
    case Method::DistanceWeightedMean: m_filter=std::bind(&SmoothGridProcess::distanceWeightedMean, this, _1, _2);break;
    case Method::TrimmedMean: m_filter=std::bind(&SmoothGridProcess::trimmedMean, this, _1, _2); break;
    case Method::Median: m_filter=std::bind(&SmoothGridProcess::median, this, _1, _2); break;
    }

    //load input grid
    m_inputGrid=project()->loadGrid( m_gridType, m_inputName);
    if( !m_inputGrid ){
        setErrorString("Loading grid failed!");
        return ResultCode::Error;
    }

    Grid2DBounds bounds=m_inputGrid->bounds();
    m_outputGrid=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(bounds));

    if( !m_outputGrid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }


    return ResultCode::Ok;
}


ProjectProcess::ResultCode SmoothGridProcess::run(){

    Grid2DBounds bounds=m_inputGrid->bounds();

    emit currentTask("Iterating cdps");
    emit started(bounds.height());
    qApp->processEvents();

    //stupid simple way just for testing...

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        #pragma omp parallel for
        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            auto ival=(*m_inputGrid)(i,j);
            if( m_retainValues  && ival!=m_inputGrid->NULL_VALUE){
                (*m_outputGrid)(i,j)=ival;
            }
            else{
                m_filter(i,j);
            }
        }

        emit progress(i-bounds.i1());
        qApp->processEvents();
    }


    emit currentTask("Saving result grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    if( !project()->addGrid( m_gridType, m_outputName, m_outputGrid, params() )){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}


void SmoothGridProcess::mean(int i, int j){

    auto bounds=m_inputGrid->bounds();
    int imin=std::max( bounds.i1(), i-m_halfIlines);
    int imax=std::min( bounds.i2(), i+m_halfIlines);
    int jmin=std::max( bounds.j1(), j-m_halfXlines);
    int jmax=std::min( bounds.j2(), j+m_halfXlines);
    double sum=0;
    unsigned n=0;

    for( int ii=imin; ii<=imax; ii++ ){
        for( int jj=jmin; jj<jmax; jj++){
            float val = (*m_inputGrid)(ii,jj);
            if( val == m_inputGrid->NULL_VALUE) continue;
            sum+=val;
            n++;
        }
    }

    if(n){
        (*m_outputGrid)(i,j)=sum/n;
    }
}


void SmoothGridProcess::distanceWeightedMean(int i, int j){

    auto bounds=m_inputGrid->bounds();
    int imin=std::max( bounds.i1(), i-m_halfIlines);
    int imax=std::min( bounds.i2(), i+m_halfIlines);
    int jmin=std::max( bounds.j1(), j-m_halfXlines);
    int jmax=std::min( bounds.j2(), j+m_halfXlines);
    double sum=0;
    double wsum=0;

    for( int ii=imin; ii<=imax; ii++ ){
        for( int jj=jmin; jj<jmax; jj++){
            float val = (*m_inputGrid)(ii,jj);
            if( val == m_inputGrid->NULL_VALUE) continue;
            auto di=ii-i;
            auto dj=jj-j;
            double dist=std::sqrt( di*di + dj*dj);
            auto w = (dist>0) ? 1./dist : 1;
            sum+=w*val;
            wsum+=w;
        }
    }

    if(wsum > 0){
        (*m_outputGrid)(i,j)=sum/wsum;
    }
}

void SmoothGridProcess::trimmedMean(int i, int j){

    auto bounds=m_inputGrid->bounds();
    int imin=std::max( bounds.i1(), i-m_halfIlines);
    int imax=std::min( bounds.i2(), i+m_halfIlines);
    int jmin=std::max( bounds.j1(), j-m_halfXlines);
    int jmax=std::min( bounds.j2(), j+m_halfXlines);

    std::vector<double> buf;
    buf.reserve((imax-imin+1)*(jmax-jmin+1));
    for( int ii=imin; ii<=imax; ii++ ){
        for( int jj=jmin; jj<jmax; jj++){
            float val = (*m_inputGrid)(ii,jj);
            if( val == m_inputGrid->NULL_VALUE) continue;
            buf.push_back(val);
        }
    }

    if( buf.empty()){
        return;     // output is null anyway
    }

    std::sort(buf.begin(), buf.end());

    double keep=0.8;
    int start=0.5*(1.0-keep)*buf.size();
    int stop=buf.size()-start;  // symmetric
    double sum=std::accumulate( &buf[start], &buf[stop], 0);
    auto n=stop-start+1;

    (*m_outputGrid)(i,j)=sum/n;
}

void SmoothGridProcess::median(int i, int j){

    auto bounds=m_inputGrid->bounds();
    int imin=std::max( bounds.i1(), i-m_halfIlines);
    int imax=std::min( bounds.i2(), i+m_halfIlines);
    int jmin=std::max( bounds.j1(), j-m_halfXlines);
    int jmax=std::min( bounds.j2(), j+m_halfXlines);

    std::vector<double> buf;
    buf.reserve((imax-imin+1)*(jmax-jmin+1));

    for( int ii=imin; ii<=imax; ii++ ){
        for( int jj=jmin; jj<jmax; jj++){
            float val = (*m_inputGrid)(ii,jj);
            if( val == m_inputGrid->NULL_VALUE) continue;
            buf.push_back(val);
        }
    }

    if(!buf.empty()){

        if( buf.size() > buf.capacity()/2 ){
            auto it=buf.begin()+buf.size()/2;
            std::nth_element( buf.begin(), it, buf.end());
            (*m_outputGrid)(i,j)=*it;
        }
        else{                           // more than half the values in aperture were NULL -> median=NULL
            (*m_outputGrid)(i,j)=m_outputGrid->NULL_VALUE;
        }
    }
}

