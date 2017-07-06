#include "extracttimesliceprocess.h"


#include "utilities.h"

ExtractTimesliceProcess::ExtractTimesliceProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Extract Timeslice"), project, parent){

}


ProjectProcess::ResultCode ExtractTimesliceProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("input"))){
        setErrorString("Parameters contain no input volume!");
        return ResultCode::Error;
    }
    QString volumeName=parameters.value(QString("input"));
    m_volume=project()->loadVolume( volumeName);
    if( !m_volume ){
        setErrorString("Loading volume failed!");
        return ResultCode::Error;
    }

    if( !parameters.contains(QString("slice"))){
        setErrorString("Parameters contain no output slice!");
        return ResultCode::Error;
    }
    m_sliceName=parameters.value(QString("slice"));

    if( !parameters.contains(QString("horizon"))){

        if(!parameters.contains(QString("time"))){
            setErrorString("Parameters contain neither horizon nor time!");
            return ResultCode::Error;
        }

        m_sliceTime = parameters.value(QString("time")).toDouble();
    }
    else{
        QString horizonName=parameters.value( QString("horizon") );
        m_horizon=project()->loadGrid( GridType::Horizon, horizonName);
        if( !m_horizon ){
            setErrorString("Loading horizon failed!");
            return ResultCode::Error;
        }
    }


    if( m_horizon){
         m_slice=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(m_horizon->bounds()));
    }
    else{
        Grid3DBounds vbounds=m_volume->bounds();
        Grid2DBounds bounds( vbounds.inline1(), vbounds.crossline1(),
                           vbounds.inline2(), vbounds.crossline2() );
        m_slice=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(bounds));
    }

    if( !m_slice ){
        setErrorString("Allocating slice failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode ExtractTimesliceProcess::run(){

    Grid3DBounds bounds=m_volume->bounds();

    emit currentTask("Extracting Slice");
    emit started(bounds.inlineCount());
    qApp->processEvents();

    for( int i=bounds.inline1(); i<=bounds.inline2(); i++){

        for( int j=bounds.crossline1(); j<=bounds.crossline2(); j++){

            qreal t=0.001*m_sliceTime;  // slicetime in millisecs
            if( m_horizon){ // we are using a horizon and not constant time

                auto v=m_horizon->valueAt(i, j);
                if( v == m_horizon->NULL_VALUE ) continue;

                t=0.001 * v;    // horizon in millis
            }

            (*m_slice)(i, j)=m_volume->value(i,j,t);

        }

        emit progress( i - bounds.inline1());
        qApp->processEvents();
        if( isCanceled()) return ResultCode::Canceled;
    }


    std::pair<GridType, QString> sliceTypeAndName = splitFullGridName( m_sliceName );
    if( !project()->addGrid( sliceTypeAndName.first, sliceTypeAndName.second, m_slice)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_sliceName) );
        return ResultCode::Error;
    }

    emit finished();

    return ResultCode::Ok;
}

