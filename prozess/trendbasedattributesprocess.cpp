#include "trendbasedattributesprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

TrendBasedAttributesProcess::TrendBasedAttributesProcess( std::shared_ptr<AVOProject> project, QObject* parent) :
    ProjectProcess( QString("Trend Based Attributes"), project, parent){

}

ProjectProcess::ResultCode TrendBasedAttributesProcess::init( const QMap<QString, QString>& parameters ){

    using namespace std::placeholders;

    if( !parameters.contains(QString("output"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_outputName=parameters.value(QString("output"));


    if( !parameters.contains(QString("intercept"))){

        setErrorString("Parameters contain no intercept grid!");
        return ResultCode::Error;
    }
    m_interceptName=parameters.value( QString("intercept") );

    m_intercept=project()->loadGrid( GridType::Attribute, m_interceptName);
    if( !m_intercept ){
        setErrorString("Loading intercept grid failed!");
        return ResultCode::Error;
    }


    if( !parameters.contains(QString("gradient"))){

        setErrorString("Parameters contain no gradient grid!");
        return ResultCode::Error;
    }

    m_gradientName=parameters.value( QString("gradient") );
    m_gradient=project()->loadGrid( GridType::Attribute, m_gradientName);
    if( !m_gradient ){
        setErrorString("Loading gradient grid failed!");
        return ResultCode::Error;
    }

    
    if( !parameters.contains(QString("type"))){

        setErrorString("Parameters contain no type!");
        return ResultCode::Error;
    }
    int type=parameters.value("type").toInt();

    switch(type){

    case 0: m_func= std::bind(&TrendBasedAttributesProcess::FF, this, _1, _2) ; break;
    case 1: m_func= std::bind(&TrendBasedAttributesProcess::LF, this, _1, _2) ; break;
    case 2: m_func=std::bind(&TrendBasedAttributesProcess::PF, this, _1, _2) ; break;
    case 3: m_func= std::bind(&TrendBasedAttributesProcess::AC, this, _1, _2) ; break;
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

    Grid2DBounds bounds=m_intercept->bounds();
    m_grid=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(bounds));


    if( !m_grid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode TrendBasedAttributesProcess::run(){


    Grid2DBounds bounds=m_intercept->bounds();      // iall input grids have same bounds, checked on init
    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero
    QVector<QPointF> all;


    emit currentTask("Computing output grid");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            float G=(*m_gradient)(i,j);
            float I=(*m_intercept)(i,j);

            if( G==m_gradient->NULL_VALUE || I==m_intercept->NULL_VALUE){

               (*m_grid)(i,j) = m_grid->NULL_VALUE;
            }
            else{
                (*m_grid)(i,j)=m_func(I,G);
            }
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    emit currentTask("Saving result grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    std::pair<GridType, QString> gridTypeAndName = splitFullGridName( m_outputName );
    if( !project()->addGrid( gridTypeAndName.first, gridTypeAndName.second, m_grid)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}


float TrendBasedAttributesProcess::FF(float I, float G ){

    return (I*m_sinPhi  + G*m_cosPhi) - m_trendIntercept *m_cosPhi;  // for trends not going through origin
}

float TrendBasedAttributesProcess::LF(float I, float G){

    float G0=m_trendIntercept;
    return I*m_cosPhi - ( G - G0)*m_sinPhi;
}

float TrendBasedAttributesProcess::PF(float I, float G){

    float lf=LF(I,G);

    float trendG=m_trendIntercept-I*m_tanPhi;
    if( G<trendG ){
        lf=-lf;
    }

    return lf;
}


float TrendBasedAttributesProcess::AC(float I, float G){

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



void TrendBasedAttributesProcess::computeTrend(){

    emit currentTask("Computing crossplot angle");
    emit started(100);
    qApp->processEvents();

    Grid2DBounds bounds=m_intercept->bounds();

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero
    QVector<QPointF> all;

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            double intercept=(*m_intercept)(i,j);
            if( intercept == m_intercept->NULL_VALUE) continue;

            double gradient=(*m_gradient)(i,j);
            if( gradient==m_gradient->NULL_VALUE) continue;

            all.push_back( QPointF(intercept, gradient) );
        }

        if( (i-bounds.i1()) % onePercent==0 ){
            emit progress((i-bounds.i1()) / onePercent);
            qApp->processEvents();
        }
    }

    QPointF trendInterceptAndGradient=linearRegression(all);
    m_trendAngle=std::fabs(std::atan(trendInterceptAndGradient.y() ) );
    m_trendIntercept=trendInterceptAndGradient.x();
}
