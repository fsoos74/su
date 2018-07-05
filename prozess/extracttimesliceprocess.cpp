#include "extracttimesliceprocess.h"


#include "utilities.h"

ExtractTimesliceProcess::ExtractTimesliceProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Extract Timeslice"), project, parent){

}


ProjectProcess::ResultCode ExtractTimesliceProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString inputName;
    QString horizonName;

    try{
        inputName=getParam(parameters, "input");
        m_sliceName=getParam(parameters, "slice");
        m_useHorizon=static_cast<bool>(getParam(parameters, "use-horizon").toInt());
        horizonName=getParam(parameters, "horizon");
        m_sliceTime = parameters.value(QString("time")).toDouble();

    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    auto res=addInputVolume(inputName);
    if( res!=ResultCode::Ok) return res;

    auto vbounds=inputBounds(0);     //take from volume
    Grid3DBounds bounds;

    if( m_useHorizon){
        m_horizon=project()->loadGrid( GridType::Horizon, horizonName);
        if( !m_horizon ){
            setErrorString("Loading horizon failed!");
            return ResultCode::Error;
        }
        auto hbounds=m_horizon->bounds();
        // find area covered by volume and horizon
        bounds=Grid3DBounds( std::max(hbounds.i1(),vbounds.i1()), std::min(hbounds.i2(),vbounds.i2()),
                                  std::max(hbounds.j1(),vbounds.j1()), std::min(hbounds.j2(),vbounds.j2()),
                                  vbounds.nt(), vbounds.ft(), vbounds.dt());

    }else{
        bounds=vbounds;
    }

    m_slice=std::make_shared<Grid2D<float> >( Grid2DBounds(bounds.i1(), bounds.j1(), bounds.i2(), bounds.j2()) );
    setBounds(bounds);

    if( !m_slice ){
        setErrorString("Allocating slice failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode ExtractTimesliceProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){

    auto input=inputs[0];

    for( int j=bounds().j1(); j<=bounds().j2(); j++){

        qreal t;
        if( m_horizon){ // we are using a horizon and not constant time

            auto v=m_horizon->valueAt(iline, j);
            if( v == m_horizon->NULL_VALUE ) continue;

            t=0.001 * v;    // horizon in millis
        }
        else{
            t=0.001*m_sliceTime;  // slicetime in millisecs
        }

        (*m_slice)(iline, j)=input->value(iline,j,t);

    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode ExtractTimesliceProcess::finalize(){

    std::pair<GridType, QString> sliceTypeAndName = splitFullGridName( m_sliceName );
    if( !project()->addGrid( sliceTypeAndName.first, sliceTypeAndName.second, m_slice, params() )){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_sliceName) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

