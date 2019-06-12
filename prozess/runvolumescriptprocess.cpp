
#include "runvolumescriptprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>
#include<QScriptEngine>
#include<QScriptValue>
#include<QScriptValueList>


RunVolumeScriptProcess::RunVolumeScriptProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Run Volume Script"), project, parent){

}

ProjectProcess::ResultCode RunVolumeScriptProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    if( !parameters.contains(QString("result"))){
        setErrorString("Parameters contain no output volume!");
        return ResultCode::Error;
    }
    m_volumeName=parameters.value(QString("result"));

    if( !parameters.contains(QString("n_input_volumes"))){
        setErrorString("Parameters contain no number of input volumes!");
        return ResultCode::Error;
    }
    int n=parameters.value("n_input_volumes").toInt();

    for( int i=0; i<n; i++){
        QString key=QString("input_volume_")+QString::number(i+1);
        if( !parameters.contains(key)){
            setErrorString("Parameters do not contain all input volumes!");
            return ResultCode::Error;
        }
        QString name=parameters.value(key);
        m_inputVolumeName.push_back(name);
    }

    if( !parameters.contains(QString("n_input_grids"))){
        setErrorString("Parameters contain no number of input grids!");
        return ResultCode::Error;
    }
    n=parameters.value("n_input_grids").toInt();

    for( int i=0; i<n; i++){
        QString key=QString("input_grid_")+QString::number(i+1);
        if( !parameters.contains(key)){
            setErrorString("Parameters do not contain all input grids!");
            return ResultCode::Error;
        }
        QString name=parameters.value(key);
        m_inputGridName.push_back(name);
    }

    if( !parameters.contains(QString("script"))){
        setErrorString("Parameters contain no script!");
        return ResultCode::Error;
    }
    m_script=parameters.value("script");

    //load input volumes
    for( int i=0; i<m_inputVolumeName.size(); i++){
        auto pg=project()->loadVolume( m_inputVolumeName[i]);
        if( !pg ){
            setErrorString("Loading volume failed!");
            return ResultCode::Error;
        }
        m_inputVolume.append(pg);
    }

    //load input grids
    for( int i=0; i<m_inputGridName.size(); i++){
        auto pg=project()->loadGrid( GridType::Attribute, m_inputGridName[i]);
        if( !pg ){
            setErrorString("Loading grid failed!");
            return ResultCode::Error;
        }
        m_inputGrid.append(pg);
    }

    Q_ASSERT( m_inputVolume.size()>0);

    Grid3DBounds bounds=m_inputVolume[0]->bounds();
    // verify that all grids have the correct geometry
    for( int i=1; i<m_inputVolume.size(); i++){
        if( !(m_inputVolume[i]->bounds()==bounds) ){
            setErrorString(QString::asprintf(
                 "Geometry of volume %d differs from first volume", i+1));
            return ResultCode::Error;
        }
    }


    // verify that all grids have the correct geometry
    Grid2DBounds bounds2(bounds.i1(), bounds.j1(),
                         bounds.i2(), bounds.j2());
    for( int i=0; i<m_inputGrid.size(); i++){
        if( !(m_inputGrid[i]->bounds()==bounds2) ){
            setErrorString(QString::asprintf(
                 "Geometry of grid %d differs from first volume", i+1));
            return ResultCode::Error;
        }
    }

    m_volume=std::shared_ptr<Volume >( new Volume(bounds));


    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode RunVolumeScriptProcess::run(){

    QScriptEngine engine;
    QScriptValue fun=engine.evaluate(tr("(") +m_script+ tr(")"));
    if(engine.hasUncaughtException()){
        int line = engine.uncaughtExceptionLineNumber();
        auto message=engine.uncaughtException().toString();
        setErrorString(QString("Error in line %1: %2").arg(QString::number(line), message));
        return ResultCode::Error;
    }

    Grid3DBounds bounds=m_volume->bounds();


    emit currentTask("Iterating cdps");
    emit started(bounds.ni()*bounds.nj());
    qApp->processEvents();


    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            for( int k=0; k<bounds.nt(); k++){

                // input values
                QScriptValueList args;

                // add volume values
                for( int vi=0; vi<m_inputVolume.size(); vi++){

                    // null values are not translated explicitly because isfinite is called in scripts
                    args<<(*m_inputVolume[vi])(i,j,k);
                }

                for( int gi=0; gi<m_inputGrid.size(); gi++){

                    // null values are not translated explicitly because isfinite is called in scripts
                    args<<(*m_inputGrid[gi])(i,j);
                }

                // compute output sample
                QScriptValue result = fun.call(QScriptValue(), args);
                if(engine.hasUncaughtException()){
                    setErrorString(tr("Error occured: ")+result.toString());
                    return ResultCode::Error;
                }
                auto res=result.toNumber();
                (*m_volume)(i,j,k)=std::isfinite(res) ? res : m_volume->NULL_VALUE;
            }

            emit progress((i-bounds.i1())*bounds.nj()+j);
            qApp->processEvents();
        }

    }

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_volumeName, m_volume, params() )){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }

    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}


