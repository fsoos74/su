#include "trendbasedattributevolumesprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

 using namespace std::placeholders;

TrendBasedAttributeVolumesProcess::TrendBasedAttributeVolumesProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Trend Based Attribute Volumes"), project, parent){

}

ProjectProcess::ResultCode TrendBasedAttributeVolumesProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_outputName=getParam(parameters, "output");
        m_interceptName=getParam(parameters, "intercept");
        m_gradientName=getParam(parameters, "gradient");
        m_computeTrend=static_cast<bool>(getParam(parameters,"compute-trend").toInt());
        m_trendIntercept=getParam(parameters,"trend-intercept").toFloat();
        m_trendAngle=getParam(parameters,"trend-angle").toFloat();
        m_trendAngle*=M_PI/180.0;    // convert to radian
        m_attribute=getParam(parameters,"attribute").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode TrendBasedAttributeVolumesProcess::run(){

    m_interceptReader=project()->openVolumeReader(m_interceptName);
    if(!m_interceptReader){
        setErrorString("Open intercept reader failed!");
        return ResultCode::Error;
    }

    m_gradientReader=project()->openVolumeReader(m_gradientName);
    if(!m_gradientReader){
        setErrorString("Open gradient volume failed!");
        return ResultCode::Error;
    }

    if( m_interceptReader->bounds()!=m_gradientReader->bounds()){
        setErrorString("Intercept and gradient dimensions do not match!");
        return ResultCode::Error;
    }

    m_bounds=m_interceptReader->bounds();

    if( initFunction()!=ResultCode::Ok){
        return ResultCode::Error;
    }

    if( m_computeTrend){
        auto res= computeTrend();     // will put result in m_trendIntercept and m_trendAngle
        if( res!=ResultCode::Ok) return res;
    }

    m_sinPhi=std::sin(m_trendAngle);
    m_cosPhi=std::cos(m_trendAngle);
    m_tanPhi=std::tan(m_trendAngle);

    auto writer=project()->openVolumeWriter(m_outputName, m_bounds, Domain::Other, VolumeType::AVO);
    if( !writer ){
        setErrorString("Open volume writer failed!");
        return ResultCode::Error;
    }

    emit started(m_bounds.ni());
    emit currentTask("Computing attribute...");
    qApp->processEvents();

    for( int il1=m_bounds.i1(); il1<=m_bounds.i2(); il1+=CHUNK_ILINES){

        auto il2=std::min(il1+CHUNK_ILINES, m_bounds.i2());

        auto subIntercept=m_interceptReader->read(il1,il2);
        if( !subIntercept){
            setErrorString("Reading intercept chunk failed!");
            writer->removeFile();
            return ResultCode::Error;
        }
        auto subGradient=m_gradientReader->read(il1,il2);
        if( !subGradient){
            setErrorString("Reading gradient chunk failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        auto sbounds=subIntercept->bounds();
        auto svolume=std::make_shared<Volume>(sbounds);
        if( !svolume){
            setErrorString("Creating subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        qint64 size=svolume->size();
        Q_ASSERT( subIntercept->size()==size && subGradient->size()==size);


        //  start chunk processing
        for( qint64 i=0; i<size; i++){

            auto I=(*subIntercept)[i];
            auto G=(*subGradient)[i];
            if( I==subIntercept->NULL_VALUE || G==subGradient->NULL_VALUE){
                (*svolume)[i]=svolume->NULL_VALUE;
            }
            else{
                (*svolume)[i]=m_func(I,G);
            }

        }
        // END chunk processing

        std::cout<<"output inlines: "<<svolume->bounds().i1()<<" - "<<svolume->bounds().i2()<<std::endl<<std::flush;
        if(!writer->write(*svolume)){
            setErrorString("Writing subvolume failed!");
            writer->removeFile();
            return ResultCode::Error;
        }

        subIntercept.reset();
        subGradient.reset();
        svolume.reset();

        emit progress(il2-m_bounds.i1());
        qApp->processEvents();
    }

    writer->flush();
    writer->close();

    if( !project()->addVolume( m_outputName, params() ) ){
        writer->removeFile();
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

/* original no chunks!!!
ProjectProcess::ResultCode TrendBasedAttributeVolumesProcess::run(){

    Grid3DBounds bounds=m_intercept->bounds();      // iall input grids have same bounds, checked on init
    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero
    QVector<QPointF> all;


    emit currentTask("Computing output grid");
    emit started(100);
    qApp->processEvents();

    for( int iline=bounds.i1(); iline<=bounds.i2(); iline++){

        for( int xline=bounds.j1(); xline<=bounds.j2(); xline++){

            for( int sample=0; sample<bounds.nt(); sample++){

                float G=(*m_gradient)(iline, xline, sample);
                float I=(*m_intercept)(iline, xline, sample);

                if( G==m_gradient->NULL_VALUE || I==m_intercept->NULL_VALUE){

                   (*m_volume)(iline, xline, sample) = m_volume->NULL_VALUE;
                }
                else{
                    (*m_volume)(iline, xline, sample)=m_func(I,G);
                }

             }
        }

        if( (iline-bounds.i1()) % onePercent==0 ){
            emit progress((iline-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_outputName, m_volume, params() ) ){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
*/


float TrendBasedAttributeVolumesProcess::FF(float I, float G ){

    return (I*m_sinPhi  + G*m_cosPhi) - m_trendIntercept *m_cosPhi;  // for trends not going through origin
}

float TrendBasedAttributeVolumesProcess::LF(float I, float G){

    float G0=m_trendIntercept;
    return I*m_cosPhi - ( G - G0)*m_sinPhi;
}

float TrendBasedAttributeVolumesProcess::PF(float I, float G){

    float lf=LF(I,G);

    float trendG=m_trendIntercept-I*m_tanPhi;
    if( G<trendG ){
        lf=-lf;
    }

    return lf;
}


float TrendBasedAttributeVolumesProcess::AC(float I, float G){

    float trendG=m_trendIntercept-I*m_tanPhi;
    float ac=0;

    // SEG positive polarity

    if( I*G >= 0 ){ // class III

        ac=(G>trendG ) ? 3 : -3;
    }
    else if( G > 0){
        ac= (G>trendG) ? 1: -4;
    }
    else{
        ac= (G>trendG) ? 4 : -1;
    }

    return ac;
}


ProjectProcess::ResultCode TrendBasedAttributeVolumesProcess::initFunction(){
    switch(m_attribute){

    case 0: m_func= std::bind(&TrendBasedAttributeVolumesProcess::FF, this, _1, _2) ; break;
    case 1: m_func= std::bind(&TrendBasedAttributeVolumesProcess::LF, this, _1, _2) ; break;
    case 2: m_func=std::bind(&TrendBasedAttributeVolumesProcess::PF, this, _1, _2) ; break;
    case 3: m_func= std::bind(&TrendBasedAttributeVolumesProcess::AC, this, _1, _2) ; break;
    default:
        setErrorString("Invalid attribute type!");
        return ResultCode::Error;
        break;
    }
    return ResultCode::Ok;
}


ProjectProcess::ResultCode TrendBasedAttributeVolumesProcess::computeTrend(){

    emit started(m_bounds.ni());
    emit currentTask("Computing trend...");
    qApp->processEvents();

    double sum_x=0;
    double sum_x_x=0;
    double sum_x_y=0;
    double sum_y=0;
    double sum_y_y=0;       // needed for correlation coefficient
    qint64 n=0;

    for( int il1=m_bounds.i1(); il1<=m_bounds.i2(); il1+=CHUNK_ILINES){

        auto il2=std::min(il1+CHUNK_ILINES, m_bounds.i2());
        auto subIntercept=m_interceptReader->read(il1,il2);
        auto subGradient=m_gradientReader->read(il1,il2);
        if( !subIntercept || !subGradient){
            setErrorString("Reading chunks failed!");
            return ResultCode::Error;
        }

        auto iti=subIntercept->cbegin();
        auto itg=subGradient->cbegin();
        while( iti!=subIntercept->cend() && itg!=subGradient->cend()){
            auto x=*iti++;
            auto y=*itg++;
            if( x==subIntercept->NULL_VALUE || y==subGradient->NULL_VALUE ) continue;
            sum_x+=x;
            sum_x_x+=x*x;
            sum_x_y+=x*y;
            sum_y+=y;
            sum_y_y+=y*y;
            n++;
        }

        subIntercept.reset();
        subGradient.reset();

        emit progress(il2-m_bounds.i1());
        qApp->processEvents();
    }

    if( n==0 || sum_x==0 ){
        setErrorString("No valid (non-null) input data!");
        return ResultCode::Error;
    }

    double Sxx=sum_x_x - sum_x*sum_x/n;
    double Syy=sum_y_y - sum_y*sum_y/n;
    double Sxy=sum_x_y - sum_x * sum_y / n;

    double gradient = Sxy / Sxx;
    double intercept= sum_y/n - gradient*sum_x/n;

    m_trendAngle=std::fabs(std::atan(gradient ) );
    m_trendIntercept=intercept;

   /*
    std::cout<<"computing trend...input size="<<m_intercept->size()<<std::endl<<std::flush;
    Q_ASSERT(m_intercept->size()==m_gradient->size());
    QPointF trendInterceptAndGradient=linearRegression(m_intercept->data(), m_gradient->data(), m_intercept->size(), m_intercept->NULL_VALUE);
    m_trendAngle=std::fabs(std::atan(trendInterceptAndGradient.y() ) );
    m_trendIntercept=trendInterceptAndGradient.x();
    */

    return ResultCode::Ok;
}
