#include "fmcdp2dprocess.h"

#include <avoproject.h>
#include <seismicdatasetwriter.h>
#include "utilities.h"
#include <processparams.h>
#include <omp.h>
#include<iostream>

#include<QApplication>



const size_t WPML = 20;         // width of pml boundary


//#include<array2d.h>
//using util::Array2D;
template<typename T>
class Array2D{

public:

    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    Array2D( size_t n, size_t m, const T& ival = T(0) ):
        m_n(n), m_m(m), m_data(n*m, ival){
    }

    size_t n()const{
        return m_n;
    }

    size_t m()const{
        return m_m;
    }

    size_t size()const{
        return m_data.size();
    }

    T& operator[](size_t k){
        return m_data[k];
    }

    const T& operator[](size_t k)const{
        return m_data[k];
    }

    T& operator()(size_t i, size_t j){
        return m_data[index(i,j)];
    }

    const T& operator()(size_t i, size_t j)const{
        return m_data[index(i,j)];
    }

    iterator begin(){
        return m_data.begin();
    }

    iterator end(){
        return m_data.end();
    }

    const_iterator cbegin()const{
        return m_data.cbegin();
    }

    const_iterator cend()const{
        return m_data.cend();
    }
private:

    inline size_t index(size_t i, size_t j)const{
        return i*m_m + j;
    }

    size_t m_n;
    size_t m_m;
    std::vector<T> m_data;
};


QVector<double> computeOffsets( double aoffset1, double aoffset2, unsigned n, bool split_spread){

    QVector<double> res;

    double doffset= (n>1) ? (aoffset2-aoffset1)/(n-1) : aoffset2-aoffset1;
    for( size_t i=0; i<n; i++){
        res.push_back(aoffset1+i*doffset);
    }


    if( split_spread ){
        QVector<double> rev;

        for( auto it = res.rbegin(); it!=res.rend(); it++ ){
            rev.push_back( - *it);
        }

        for( auto it=res.begin(); it!=res.end(); it++){
            rev.push_back(*it);
        }

        res = rev;
    }

    return res;
}


FMCDP2DProcess::FMCDP2DProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("FM CDP 2D (Acoustic)"), project, parent){

}

ProjectProcess::ResultCode FMCDP2DProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString model;

    QString type;

    try{
        type=getParam(parameters, "wavelet");
        m_f0=getParam(parameters, "f0").toDouble();
        m_delay=0.001*getParam(parameters, "delay").toDouble();     // msec -> sec
        m_dxz=getParam(parameters, "dxz").toDouble();
        m_tmax=0.001*getParam(parameters, "tmax").toDouble();       // msec-> sec
        m_offset1=getParam(parameters, "offset1").toDouble();
        m_offset2=getParam(parameters, "offset2").toDouble();
        m_nrc=getParam(parameters, "nrc").toUInt();
        m_split_spread=static_cast<bool>(getParam(parameters, "split-spread").toInt());
        m_outputName=getParam(parameters, "output");
        m_model=getParam( parameters, "model");
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


#include<fstream>
std::vector<double> gaussSource( size_t nt, double dt, double f0, double delay){

    std::vector<double> src(nt);
    std::vector<double> src2(nt);

    // build source function values
    for( size_t it=0; it<nt; it++){
        double t=it*dt;
        src[it]=std::exp( -1.*std::pow(f0*(t-delay),2) );  // beware of negative difference with unsigned size_t
    }
    // first derivative of source function
    for( size_t it=1; it+1<nt; it++){
        src[it]=(src[it]-src[it+1])/dt;
    }
    src[0]=0;
    src[nt-1]=0;

    std::ofstream f("/home/fsoos/wavelet.txt");
    for( size_t it=0; it<nt; it++){
        f<<it*dt<<" "<<src[it]<<std::endl;
    }

    return src;
}

std::vector<double> rickerSource( size_t nt, double dt, double f0, double delay){

    std::vector<double> src(nt);

    // build source function values
    for( size_t it=0; it<nt; it++){
        double t=it*dt-delay;
        src[it] = (1.0 - 2.0*std::pow(M_PI*f0*t,2) ) * std::exp(-std::pow(M_PI*f0*t,2));
        //src[it] = (1.0 - 2.0*std::pow(M_PI*f0*t,2) ) * std::exp(-std::pow(M_PI*f0*t,2));
    }

    std::ofstream f("/home/fsoos/wavelet.txt");
    for( size_t it=0; it<nt; it++){
        f<<it<<" "<<src[it]<<std::endl;
    }
    return src;
}



std::vector<std::pair<double, double>> layersFromDescription( const QString& layerDescr){

    QStringList rows = layerDescr.split(",", QString::SkipEmptyParts);
    std::vector<std::pair<double, double>> depvels;
    for( auto row : rows ){
        auto sl = row.split(":", QString::SkipEmptyParts);
        std::vector<QString> cols{sl.begin(), sl.end()};
        if( cols.size()!=2) continue;
        auto dep = cols[0].toDouble();
        auto vel = cols[1].toDouble();
        depvels.push_back(std::make_pair(dep,vel));
    }

    return depvels;
}


Array2D<double> buildModel2D( const std::vector<std::pair<double, double>>& depvels , size_t nx, size_t nz, double dz){

    std::cout<<"build model nx="<<nx<<" nz="<<nz<<" dz="<<dz<<std::endl<<std::flush;
    size_t layer=0;
    auto depvel = depvels[layer];
    size_t bottom = static_cast<size_t>( std::ceil(depvel.first/dz));
    auto vel = depvels[layer].second;
    Array2D<double> c(nz, nx);
    for( size_t i = WPML; i<nz-WPML; i++){
        std::cout<<"layer="<<layer<<" i="<<i<<std::endl<<std::flush;
        while( (i-WPML)>bottom && layer+1<depvels.size() ){
            layer++;
            depvel = depvels[layer];
            bottom = static_cast<size_t>( std::ceil(depvel.first/dz));
            vel = depvels[layer].second;
        }

        for( size_t j=0; j<nx;j++){
            c(i,j)=vel;
        }
    }

    std::cout<<"copying into pml layers"<<std::endl<<std::flush;
    // copy first and last row into pml layer
    for( size_t i = 0; i<WPML; i++){
        for( size_t j=0; j<nx;j++){
            c(i,j)=c(WPML,j);
            c(i+nz-WPML, j)=c(nz-WPML-1,j);
        }
    }

    return c;
}



#include<fstream>
/*
// TEST with 4th order derivatives - DOES NOT WORK !!!
ProjectProcess::ResultCode FMCDP2DProcess::run(){

    //compute source-receiver offsets
    auto offsets = computeOffsets( m_offset1, m_offset2, m_nrc, m_split_spread);

    //estimate model dimensions and cell size
    double dz=m_dxz;             // model cell size in z direction
    auto dx=m_dxz;                // XXX This is cell size in x direction, m_drc is offset increment!!!
    const double lenx=offsets.back()-offsets.front();
    auto nx=static_cast<size_t>(std::ceil(lenx/dx)) + 2*WPML;     // model cells in x direction
    auto depvels = layersFromDescription(m_model);
    size_t nz = static_cast<size_t>(1 + std::ceil( depvels.back().first / dz)) + 2*WPML;

    // build layerd velocity model
    std::cout<<"building velocity model"<<std::endl<<std::flush;
    auto c = buildModel2D( depvels, nx, nz, dz );   // vp velocity model

    // estimate maximum velocity and compute time sampling
    const double cmax=*std::max_element(c.begin(), c.end());
    auto dt=0.7*dx/cmax/2;
    auto nt=static_cast<size_t>(std::ceil(m_tmax/dt));

    std::cout<<"dx="<<dx<<" dz="<<dz<<" dt="<<dt<<std::endl<<std::flush;
    std::cout<<"nx="<<nx<<" nz="<<nz<<" nt="<<nt<<std::endl<<std::flush;

    std::cout<<"building damping arrays"<<std::endl<<std::flush;

    // build pml damping arrays
    Array2D<double> sigmax(nz,nx,0);
    Array2D<double> sigmaz(nz,nx,0);
    std::cout<<"Damping arrays created"<<std::endl<<std::flush;

    for( size_t k = 0; k<WPML; k++){
        auto s = 3.*cmax/(2.*dx*WPML)*std::pow((1.-double(k)/WPML),2)*7; // use 7 with wpml=20, 5 with 10
        for( size_t i = 0; i<nz; i++){
            sigmax(i,k) = s;
            sigmax(i,nx-1-k) = s;
        }
        for( size_t j=0; j<nx; j++){
            sigmaz(k,j) =s;
            sigmaz(nz-1-k,j) = s;
        }
    }
    std::cout<<"Damping arrays computed"<<std::endl<<std::flush;

    size_t isx = (m_split_spread) ? nx/2 : WPML + 1;            // source x position in model
    std::vector<size_t> rx;                                     // receiver x positions

    // compute receiver x positions
    for( auto offset : offsets ){
        int offset_cells = offset/m_dxz;
        int rr=static_cast<int>(isx) + offset_cells;
        std::cout<<"offset="<<offset<<" offset_cells="<<offset_cells<<" rr="<<rr<<std::endl<<std::flush;
        if( rr < static_cast<int>(WPML) || rr > static_cast<int>(nx - WPML) ) continue;
        rx.push_back(static_cast<size_t>(rr));      // rr>0
        std::cout<<"rx="<<rx.back()<<std::endl<<std::flush;
    }

    const size_t isz=WPML+1;             // source z position in grid
    const size_t rz=isz;            // receiver z position in grid

    auto src=gaussSource( nt, dt, m_f0, m_delay);
    //auto src=rickerSource( nt, dt, m_f0, m_delay);

   Array2D<double> p(nz,nx,0);
   Array2D<double> px(nz,nx,0);
   Array2D<double> pz(nz,nx,0);
   Array2D<double> dxpx(nz,nx,0);
   Array2D<double> dzpx(nz,nx,0);
   Array2D<double> dxpz(nz,nx,0);
   Array2D<double> dzpz(nz,nx,0);
   Array2D<double> pxnew(nz,nx,0);
   Array2D<double> pznew(nz,nx,0);
   Array2D<double> vx(nz,nx,0);
   Array2D<double> vz(nz,nx,0);
   Array2D<double> dxvx(nz,nx,0);
   Array2D<double> dzvx(nz,nx,0);
   Array2D<double> dxvz(nz,nx,0);
   Array2D<double> dzvz(nz,nx,0);
   Array2D<double> vxnew(nz,nx,0);
   Array2D<double> vznew(nz,nx,0);
   Array2D<double> seis(nt,rx.size());    // seismograms at receivers

   emit currentTask("Computing output traces");
   emit started(nt);
   qApp->processEvents();

    // iterate
   for( size_t it=0; it<nt; it++){

        // inject source
        px(isz,isx) += src[it];
        pz(isz,isx) += src[it];

        // compute velocity derivatives
        for( int i=2; i<static_cast<int>(nz)-2; i++ ){
            #pragma omp parallel for
            for( int j=2; j<static_cast<int>(nx)-2; j++){
                dzvx(i,j)=(-vx(i+2,j)+8*vx(i+1,j)-8*vx(i-1,j)+vx(i-2,j))/12;
                dzvz(i,j)=(-vz(i+2,j)+8*vz(i+1,j)-8*vz(i-1,j)+vz(i-2,j))/12;
            }
        }

        for( int j=2; j<static_cast<int>(nx)-2; j++ ){
            #pragma omp parallel for
            for( int i=2; i<static_cast<int>(nz-2); i++){
                dxvx(i,j)=(-vx(i,j+2)+8*vx(i,j+1)-8*vx(i,j-1)+vx(i,j-2))/12;
                dxvz(i,j)=(-vz(i,j+2)+8*vz(i,j+1)-8*vz(i,j-1)+vz(i,j-2))/12;
            }
        }

        // compute updated x/z pressuere components
        #pragma omp parallel for
        for( int k=0; k<static_cast<int>(p.size()); k++){
            pznew[k] = (dt/dz) * std::pow(c[k],2) * dzvz[k] + (1. - dt*sigmaz[k])*pz[k];
            pxnew[k] = (dt/dx) * std::pow(c[k],2) * dxvx[k] + (1. - dt*sigmax[k])*px[k];
        }

        // better use move
        px=pxnew;
        pz=pznew;

        // compute pressure derivatives
        for( int i=2; i<static_cast<int>(nz)-2; i++ ){
            #pragma omp parallel for
            for( int j=2; j<static_cast<int>(nx)-2; j++){
                dzpx(i,j)=(-px(i+2,j)+8*px(i+1,j)-8*px(i-1,j)+px(i+2,j))/12;
                dzpz(i,j)=(-pz(i+2,j)+8*pz(i+1,j)-8*pz(i-1,j)+pz(i-2,j))/12;
            }
        }

        for( int j=2; j<static_cast<int>(nx)-2; j++ ){
            #pragma omp parallel for
            for( int i=2; i<static_cast<int>(nz-2); i++){
                dxpx(i,j)=(-px(i,j+2)+8*px(i,j+1)-8*px(1,j-1)+px(i,j-2))/12;
                dxpz(i,j)=(-pz(i,j+2)+8*pz(i,j+1)-8*pz(i,j-1)+pz(i,j-2))/12;
            }
        }

        // compute updated x/z velocity components and total pressure
        #pragma omp parallel for
        for( int k=0; k<static_cast<int>(p.size()); k++){
            vznew[k] = (dt/dz)*(dzpx[k]+dzpz[k]) + (1. - dt*sigmaz[k])*vz[k];
            vxnew[k] = (dt/dx)*(dxpx[k]+dxpz[k]) + (1. - dt*sigmax[k])*vx[k];
            p[k]=px[k]+pz[k];
        }

        // better use move
        vx=vxnew;
        vz=vznew;

        // store samples in seismograms
        for( size_t ir = 0; ir<rx.size(); ir++){
           seis(it,ir)=p(rz, rx[ir] );
        }

       emit progress(it);
       qApp->processEvents();
       if( isCanceled()) return ResultCode::Canceled;
    }

    // store gather  as dataset

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                              SeismicDatasetInfo::Domain::Time, SeismicDatasetInfo::Mode::Prestack,
                                                              0, dt, nt);

    auto writer = std::make_shared<SeismicDatasetWriter>( dsinfo );
    if( !writer ){
        setErrorString("Open writer failed!");
        return ResultCode::Error;
    }
    seismic::Trace trc( 0, dt, nt );
    seismic::Trace::Samples& sam=trc.samples();
    seismic::Header& th=trc.header();

    // ALL THESE HEADER WORDS ARE MANDATORY for dataset writer!!! dt,nt will be added
    th["cdp"]=seismic::HeaderValue::makeIntValue(1);
    th["iline"]=seismic::HeaderValue::makeIntValue(1);
    th["xline"]=seismic::HeaderValue::makeIntValue(1);
    th["cdpx"]=seismic::HeaderValue::makeFloatValue(0);
    th["cdpy"]=seismic::HeaderValue::makeFloatValue(0);
    th["offset"]=seismic::HeaderValue::makeFloatValue(0);

    for( size_t ir=0; ir<rx.size(); ir++){

        th["offset"]=seismic::HeaderValue::makeFloatValue(offsets[ir]);

        for( size_t k=0; k<nt; k++){
            sam[k]=seis(k, ir);
        }

        writer->writeTrace(trc);
    }

    writer->close();


    if( !project()->addSeismicDataset( m_outputName, dsinfo, params() )){
        setErrorString("Adding dataset to project failed!");
        project()->removeSeismicDataset(m_outputName);          // clear traces
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
*/

// working version with pml 20.1.2018
ProjectProcess::ResultCode FMCDP2DProcess::run(){

    //compute source-receiver offsets
    auto offsets = computeOffsets( m_offset1, m_offset2, m_nrc, m_split_spread);

    //estimate model dimensions and cell size
    double dz=m_dxz;             // model cell size in z direction
    auto dx=m_dxz;                // XXX This is cell size in x direction, m_drc is offset increment!!!
    const double lenx=offsets.back()-offsets.front();
    auto nx=static_cast<size_t>(std::ceil(lenx/dx)) + 2*WPML;     // model cells in x direction
    auto depvels = layersFromDescription(m_model);
    size_t nz = static_cast<size_t>(1 + std::ceil( depvels.back().first / dz)) + 2*WPML;

    // build layerd velocity model
    std::cout<<"building velocity model"<<std::endl<<std::flush;
    auto c = buildModel2D( depvels, nx, nz, dz );   // vp velocity model

    // estimate maximum velocity and compute time sampling
    const double cmax=*std::max_element(c.begin(), c.end());
    auto dt=0.7*dx/cmax/2;
    auto nt=static_cast<size_t>(std::ceil(m_tmax/dt));

    std::cout<<"dx="<<dx<<" dz="<<dz<<" dt="<<dt<<std::endl<<std::flush;
    std::cout<<"nx="<<nx<<" nz="<<nz<<" nt="<<nt<<std::endl<<std::flush;

    std::cout<<"building damping arrays"<<std::endl<<std::flush;

    // build pml damping arrays
    Array2D<double> sigmax(nz,nx,0);
    Array2D<double> sigmaz(nz,nx,0);
    std::cout<<"Damping arrays created"<<std::endl<<std::flush;

    for( size_t k = 0; k<WPML; k++){
        auto s = 3.*cmax/(2.*dx*WPML)*std::pow((1.-double(k)/WPML),2)*7; // use 7 with wpml=20, 5 with 10
        for( size_t i = 0; i<nz; i++){
            sigmax(i,k) = s;
            sigmax(i,nx-1-k) = s;
        }
        for( size_t j=0; j<nx; j++){
            sigmaz(k,j) =s;
            sigmaz(nz-1-k,j) = s;
        }
    }
    std::cout<<"Damping arrays computed"<<std::endl<<std::flush;

    size_t isx = (m_split_spread) ? nx/2 : WPML + 1;            // source x position in model
    std::vector<size_t> rx;                                     // receiver x positions

    // compute receiver x positions
    for( auto offset : offsets ){
        int offset_cells = offset/m_dxz;
        int rr=static_cast<int>(isx) + offset_cells;
        std::cout<<"offset="<<offset<<" offset_cells="<<offset_cells<<" rr="<<rr<<std::endl<<std::flush;
        if( rr < static_cast<int>(WPML) || rr > static_cast<int>(nx - WPML) ) continue;
        rx.push_back(static_cast<size_t>(rr));      // rr>0
        std::cout<<"rx="<<rx.back()<<std::endl<<std::flush;
    }

    const size_t isz=WPML+1;             // source z position in grid
    const size_t rz=isz;            // receiver z position in grid

    auto src=gaussSource( nt, dt, m_f0, m_delay);
    //auto src=rickerSource( nt, dt, m_f0, m_delay);

   Array2D<double> p(nz,nx,0);
   Array2D<double> px(nz,nx,0);
   Array2D<double> pz(nz,nx,0);
   Array2D<double> dxpx(nz,nx,0);
   Array2D<double> dzpx(nz,nx,0);
   Array2D<double> dxpz(nz,nx,0);
   Array2D<double> dzpz(nz,nx,0);
   Array2D<double> pxnew(nz,nx,0);
   Array2D<double> pznew(nz,nx,0);
   Array2D<double> vx(nz,nx,0);
   Array2D<double> vz(nz,nx,0);
   Array2D<double> dxvx(nz,nx,0);
   Array2D<double> dzvx(nz,nx,0);
   Array2D<double> dxvz(nz,nx,0);
   Array2D<double> dzvz(nz,nx,0);
   Array2D<double> vxnew(nz,nx,0);
   Array2D<double> vznew(nz,nx,0);
   Array2D<double> seis(nt,rx.size());    // seismograms at receivers

   emit currentTask("Computing output traces");
   emit started(nt);
   qApp->processEvents();

    // iterate
   for( size_t it=0; it<nt; it++){

        // inject source
        px(isz,isx) += src[it];
        pz(isz,isx) += src[it];

        // compute velocity derivatives
        for( int i=1; i<static_cast<int>(nz)-1; i++ ){
            #pragma omp parallel for
            for( int j=1; j<static_cast<int>(nx)-1; j++){
                dzvx(i,j)=vx(i+1,j)-vx(i,j);
                dzvz(i,j)=vz(i+1,j)-vz(i,j);
            }
        }

        for( int j=1; j<static_cast<int>(nx)-1; j++ ){
            #pragma omp parallel for
            for( int i=1; i<static_cast<int>(nz-1); i++){
                dxvx(i,j)=vx(i,j+1)-vx(i,j);
                dxvz(i,j)=vz(i,j+1)-vz(i,j);
            }
        }

        // compute updated x/z pressuere components
        #pragma omp parallel for
        for( int k=0; k<static_cast<int>(p.size()); k++){
            pznew[k] = (dt/dz) * std::pow(c[k],2) * dzvz[k] + (1. - dt*sigmaz[k])*pz[k];
            pxnew[k] = (dt/dx) * std::pow(c[k],2) * dxvx[k] + (1. - dt*sigmax[k])*px[k];
        }

        // better use move
        px=pxnew;
        pz=pznew;

        // compute pressure derivatives
        for( int i=1; i<static_cast<int>(nz)-1; i++ ){
            #pragma omp parallel for
            for( int j=1; j<static_cast<int>(nx)-1; j++){
                dzpx(i,j)=px(i,j)-px(i-1,j);
                dzpz(i,j)=pz(i,j)-pz(i-1,j);
            }
        }

        for( int j=1; j<static_cast<int>(nx)-1; j++ ){
            #pragma omp parallel for
            for( int i=1; i<static_cast<int>(nz-1); i++){
                dxpx(i,j)=px(i,j)-px(i,j-1);
                dxpz(i,j)=pz(i,j)-pz(i,j-1);
            }
        }

        // compute updated x/z velocity components and total pressure
        #pragma omp parallel for
        for( int k=0; k<static_cast<int>(p.size()); k++){
            vznew[k] = (dt/dz)*(dzpx[k]+dzpz[k]) + (1. - dt*sigmaz[k])*vz[k];
            vxnew[k] = (dt/dx)*(dxpx[k]+dxpz[k]) + (1. - dt*sigmax[k])*vx[k];
            p[k]=px[k]+pz[k];
        }

        // better use move
        vx=vxnew;
        vz=vznew;

        // store samples in seismograms
        for( size_t ir = 0; ir<rx.size(); ir++){
           seis(it,ir)=p(rz, rx[ir] );
        }

       emit progress(it);
       qApp->processEvents();
       if( isCanceled()) return ResultCode::Canceled;
    }

    // store gather  as dataset

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                              3, SeismicDatasetInfo::Domain::Time, SeismicDatasetInfo::Mode::Prestack,
                                                              0, dt, nt);

    auto writer = std::make_shared<SeismicDatasetWriter>( dsinfo );
    if( !writer ){
        setErrorString("Open writer failed!");
        return ResultCode::Error;
    }
    seismic::Trace trc( 0, dt, nt );
    seismic::Trace::Samples& sam=trc.samples();
    seismic::Header& th=trc.header();

    // ALL THESE HEADER WORDS ARE MANDATORY for dataset writer!!! dt,nt will be added
    th["cdp"]=seismic::HeaderValue::makeIntValue(1);
    th["iline"]=seismic::HeaderValue::makeIntValue(1);
    th["xline"]=seismic::HeaderValue::makeIntValue(1);
    th["cdpx"]=seismic::HeaderValue::makeFloatValue(0);
    th["cdpy"]=seismic::HeaderValue::makeFloatValue(0);
    th["offset"]=seismic::HeaderValue::makeFloatValue(0);

    for( size_t ir=0; ir<rx.size(); ir++){

        th["offset"]=seismic::HeaderValue::makeFloatValue(offsets[ir]);

        for( size_t k=0; k<nt; k++){
            sam[k]=seis(k, ir);
        }

        writer->writeTrace(trc);
    }

    writer->close();


    if( !project()->addSeismicDataset( m_outputName, dsinfo, params() )){
        setErrorString("Adding dataset to project failed!");
        project()->removeSeismicDataset(m_outputName);          // clear traces
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


/*
ProjectProcess::ResultCode FMCDP2DProcess::run(){

    size_t nb=45;   // sponge layer width

    double dz=m_dxz;             // model cell size in z direction
    auto dx=m_dxz;                // XXX This is cell size in x direction, m_drc is offset increment!!!

    const double padlen=3000.*m_tmax; // distance after which side reflections are impossible, use cmax of 3000 better use sponge
    const double lenx=2*(m_nrc*m_drc + padlen);      // model horizontal size with padding, split spread source in center
    auto nx=2*static_cast<size_t>(std::ceil(lenx/dx));     // model cells in x direction        // add padding against side reflections

    auto depvels = layersFromDescription(m_model);
    size_t nz = static_cast<size_t>(1 + std::ceil(depvels.back().first / dz));

    //nx+=2*nb;
    //nz+=2*nb;

    auto c = buildModel2D( depvels, nx, nz, dz );   // vp velocity model
    //addSpongeLayer(c, nb);

    const double cmax=*std::max_element(c.begin(), c.end());
    m_dt=0.7*dx/cmax;
    m_nt=static_cast<size_t>(std::ceil(m_tmax/m_dt));


    std::cout<<"dx="<<dx<<" dz="<<dz<<" dt="<<m_dt<<std::endl<<std::flush;
    std::cout<<"nx="<<nx<<" nz="<<nz<<" nt="<<m_nt<<std::endl<<std::flush;


    size_t isx = nx/2;                                  // source x position in middle of model
    std::vector<size_t> rx(m_nrc);                       // receiver x positions
    // create receiver single spread configuration
    for( size_t ir = 0; ir<m_nrc; ir++){
        auto dist=ir*m_drc;
        rx[ir]=isx + static_cast<size_t>(dist/dx);    // source in middle of model
        std::cout<<"ir="<<ir<<" rx="<<rx[ir]<<std::endl<<std::flush;
    }

    const size_t isz=1;             // source z position in grid
    const size_t rz=isz;            // receiver z position in grid

    auto src=gaussSource( m_nt, m_dt, m_f0, 100);

   Array2D<double> p(nz,nx,0);     // current pressure field
   Array2D<double> pold(nz,nx,0);  // previous timestep pressure field
   Array2D<double> pnew(nz,nx,0);  // new pressure field
   Array2D<double> pxx(nz,nx,0);   // second derivative (x) of pressure field
   Array2D<double> pzz(nz,nx,0);   // second derivative (z) of pressure field
   Array2D<double> seis(m_nt,m_nrc);    // seismograms at receivers

   emit currentTask("Computing output traces");
   emit started(m_nt);
   qApp->processEvents();

    // iterate
   for( size_t it=0; it<m_nt; it++){



       // compute partial derivatives

       // x-direction
       for( size_t j=1; j<nx-1; j++ ){
           #pragma omp parallel for
           for( size_t i=0; i<nz; i++){
               pxx(i,j)=( p(i,j+1)-2*p(i,j) +p(i,j-1))/(dx*dx);
           }
       }

       // z-direction
       for( size_t i=1; i<nz-1; i++){
           #pragma omp parallel for
           for( size_t j=0; j<nx; j++){
               pzz(i,j)=( p(i-1,j)-2*p(i,j)+p(i+1,j))/(dz*dz);
           }
       }



       // time extrapolation
       #pragma omp parallel for
       for( size_t k=0; k<nx*nz; k++){     // all fields have the same size
           pnew[k]=2 * p[k] - pold[k] + std::pow( c[k]*m_dt, 2) * (pxx[k]+pzz[k]);
       }




       // inject source
       pnew(isz,isx) += src[it];


       // store sample in seismograms
       for( size_t ir = 0; ir<m_nrc; ir++){
           seis(it,ir)=pnew(rz, rx[ir] );
       }


       auto minmax=std::minmax_element(p.begin(), p.end());
       //std::cout<<"t="<<it*m_dt<<" min="<<*minmax.first<<" max="<<*minmax.second<<std::endl<<std::flush;

       // prepare for next iteration
       pold=p;
       p=pnew;

       emit progress(it);
       qApp->processEvents();
       if( isCanceled()) return ResultCode::Canceled;
    }

    // store gather  as dataset

    SeismicDatasetInfo dsinfo = project()->genericDatasetInfo( m_outputName,
                                                              SeismicDatasetInfo::Domain::Time, SeismicDatasetInfo::Mode::Prestack,
                                                              0, m_dt, m_nt);

    auto writer = std::make_shared<SeismicDatasetWriter>( dsinfo );
    if( !writer ){
        setErrorString("Open writer failed!");
        return ResultCode::Error;
    }
    seismic::Trace trc( 0, m_dt, m_nt );
    seismic::Trace::Samples& sam=trc.samples();
    seismic::Header& th=trc.header();

    // ALL THESE HEADER WORDS ARE MANDATORY for dataset writer!!! dt,nt will be added
    th["cdp"]=seismic::HeaderValue::makeIntValue(1);
    th["iline"]=seismic::HeaderValue::makeIntValue(1);
    th["xline"]=seismic::HeaderValue::makeIntValue(1);
    th["cdpx"]=seismic::HeaderValue::makeFloatValue(0);
    th["cdpy"]=seismic::HeaderValue::makeFloatValue(0);
    th["offset"]=seismic::HeaderValue::makeFloatValue(0);

    for( size_t ir=0; ir<m_nrc; ir++){

        th["offset"]=seismic::HeaderValue::makeFloatValue(ir*m_drc);

        for( size_t k=0; k<m_nt; k++){
            sam[k]=seis(k, ir);
        }

        writer->writeTrace(trc);
    }

    writer->close();


    if( !project()->addSeismicDataset( m_outputName, dsinfo, params() )){
        setErrorString("Adding dataset to project failed!");
        project()->removeSeismicDataset(m_outputName);          // clear traces
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
*/
