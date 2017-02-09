#include "trendbasedattributevolumesprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

TrendBasedAttributeVolumesProcess::TrendBasedAttributeVolumesProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Trend Based Attribute Volumes"), project, parent){

}

ProjectProcess::ResultCode TrendBasedAttributeVolumesProcess::init( const QMap<QString, QString>& parameters ){

    using namespace std::placeholders;

    if( !parameters.contains(QString("output"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_outputName=parameters.value(QString("output"));


    if( !parameters.contains(QString("intercept"))){

        setErrorString("Parameters contain no intercept volume!");
        return ResultCode::Error;
    }
    m_interceptName=parameters.value( QString("intercept") );

    m_intercept=project()->loadVolume( m_interceptName);
    if( !m_intercept ){
        setErrorString("Loading intercept volume failed!");
        return ResultCode::Error;
    }


    if( !parameters.contains(QString("gradient"))){

        setErrorString("Parameters contain no gradient volume!");
        return ResultCode::Error;
    }

    m_gradientName=parameters.value( QString("gradient") );
    m_gradient=project()->loadVolume( m_gradientName);
    if( !m_gradient ){
        setErrorString("Loading gradient volume failed!");
        return ResultCode::Error;
    }

    
    if( !parameters.contains(QString("type"))){

        setErrorString("Parameters contain no type!");
        return ResultCode::Error;
    }
    int type=parameters.value("type").toInt();

    switch(type){

    case 0: m_func= std::bind(&TrendBasedAttributeVolumesProcess::FF, this, _1, _2) ; break;
    case 1: m_func= std::bind(&TrendBasedAttributeVolumesProcess::LF, this, _1, _2) ; break;
    case 2: m_func=std::bind(&TrendBasedAttributeVolumesProcess::PF, this, _1, _2) ; break;
    case 3: m_func= std::bind(&TrendBasedAttributeVolumesProcess::AC, this, _1, _2) ; break;
    default:
        setErrorString("Invalid attribute type!");
        return ResultCode::Error;
        break;
    }

    if( !(m_intercept->bounds()==m_gradient->bounds()) ){
        setErrorString("Intercept and gradient grids have different geometry!");
        return ResultCode::Error;
    }

    
    if( !parameters.contains(QString("compute-trend"))){

        setErrorString("Parameters contain no compute-trend!");
        return ResultCode::Error;
    }

    bool compute=parameters.value("compute-trend").toInt();

    if( compute ){
        computeTrend();
    }
    else{

        if( !parameters.contains(QString("trend-intercept"))){

            setErrorString("Parameters contain no trend-intercept!");
            return ResultCode::Error;
        }
        m_trendIntercept=parameters.value("trend-intercept").toFloat();

        if( !parameters.contains(QString("trend-angle"))){

            setErrorString("Parameters contain no trend-angle!");
            return ResultCode::Error;
        }
        m_trendAngle=parameters.value("trend-angle").toFloat();

        m_trendAngle*=M_PI/180.0;    // convert to radian
    }

    m_sinPhi=std::sin(m_trendAngle);
    m_cosPhi=std::cos(m_trendAngle);
    m_tanPhi=std::tan(m_trendAngle);

    Grid3DBounds bounds=m_intercept->bounds();
    m_volume=std::shared_ptr<Grid3D<float> >( new Grid3D<float>(bounds));


    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode TrendBasedAttributeVolumesProcess::run(){

    Grid3DBounds bounds=m_intercept->bounds();      // iall input grids have same bounds, checked on init
    int onePercent=(bounds.inline2()-bounds.inline1()+1)/100 + 1; // adding one to avoids possible division by zero
    QVector<QPointF> all;


    emit currentTask("Computing output grid");
    emit started(100);
    qApp->processEvents();

    for( int iline=bounds.inline1(); iline<=bounds.inline2(); iline++){

        for( int xline=bounds.crossline1(); xline<=bounds.crossline2(); xline++){

            for( int sample=0; sample<bounds.sampleCount(); sample++){

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

        if( (iline-bounds.inline1()) % onePercent==0 ){
            emit progress((iline-bounds.inline1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_outputName, m_volume)){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}



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



void TrendBasedAttributeVolumesProcess::computeTrend(){

    emit currentTask("Computing crossplot angle");
    emit started(100);
    qApp->processEvents();

    Grid3DBounds bounds=m_intercept->bounds();

    int onePercent=(bounds.inline2()-bounds.inline1()+1)/100 + 1; // adding one to avoids possible division by zero
    QVector<QPointF> all;

    for( int iline=bounds.inline1(); iline<=bounds.inline2(); iline++){

        for( int xline=bounds.crossline1(); xline<=bounds.crossline2(); xline++){

            for( int sample=0; sample<bounds.sampleCount(); sample++){

                float intercept=(*m_intercept)(iline, xline, sample );
                if( intercept == m_intercept->NULL_VALUE ) continue;


                float gradient=(*m_gradient)(iline, xline, sample );
                if( gradient==m_gradient->NULL_VALUE ) continue;


                all.push_back( QPointF(intercept, gradient) );

            }

        }

        if( (iline-bounds.inline1()) % onePercent==0 ){
            emit progress((iline-bounds.inline1()) / onePercent);
            qApp->processEvents();
        }

    }

    QPointF trendInterceptAndGradient=linearRegression(all);
    m_trendAngle=std::fabs(std::atan(trendInterceptAndGradient.y() ) );
    m_trendIntercept=trendInterceptAndGradient.x();
}
