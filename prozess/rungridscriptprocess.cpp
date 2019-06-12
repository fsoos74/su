

#include "rungridscriptprocess.h"

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

RunGridScriptProcess::RunGridScriptProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Run Grid Script"), project, parent){

}

ProjectProcess::ResultCode RunGridScriptProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    if( !parameters.contains(QString("type"))){
        setErrorString("Parameters contain no output grid type!");
        return ResultCode::Error;
    }
    m_gridType = toGridType(parameters.value(QString("type") ) );

    if( !parameters.contains(QString("result"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_gridName=parameters.value(QString("result"));

    if( !parameters.contains(QString("n_input_grids"))){
        setErrorString("Parameters contain no number of input grids!");
        return ResultCode::Error;
    }
    int n=parameters.value("n_input_grids").toInt();

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


    //load input grids
    for( int i=0; i<m_inputGridName.size(); i++){
        auto pg=project()->loadGrid( m_gridType, m_inputGridName[i]);
        if( !pg ){
            setErrorString("Loading grid failed!");
            return ResultCode::Error;
        }
        m_inputGrid.append(pg);
    }

    Q_ASSERT( m_inputGrid.size()>0);

    Grid2DBounds bounds=m_inputGrid[0]->bounds();

    // verify that all grids have the same geometry
    for( int i=1; i<m_inputGrid.size(); i++){
        if( !(m_inputGrid[i]->bounds()==bounds) ){
            setErrorString(QString::asprintf(
                 "Geometry of grid %d differs from first grid", i+1));
            return ResultCode::Error;
        }
    }


    m_grid=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(bounds));


    if( !m_grid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }


/*
    std::cout<<"Run Grid Process Params:"<<std::endl;
    std::cout<<"result: "<<m_gridName.toStdString()<<std::endl;
    for(int i=0; i<m_inputGridName.size(); i++){
        std::cout<<"input grid "<<i+1<<": "<<m_inputGridName[i].toStdString()<<std::endl;
    }
*/
    return ResultCode::Ok;
}


ProjectProcess::ResultCode RunGridScriptProcess::run(){

    QScriptEngine engine;
    QScriptValue fun=engine.evaluate(tr("(") +m_script+ tr(")"));
    if(engine.hasUncaughtException()){
        int line = engine.uncaughtExceptionLineNumber();
        auto message=engine.uncaughtException().toString();
        setErrorString(QString("Error in line %1: %2").arg(QString::number(line), message));
        return ResultCode::Error;
    }

    Grid2DBounds bounds=m_grid->bounds();

    emit currentTask("Iterating cdps");
    emit started(bounds.ni()*bounds.nj());
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            // input values
            QScriptValueList args;

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
            (*m_grid)(i,j)=std::isfinite(res) ? res : m_grid->NULL_VALUE;

            emit progress((i-bounds.i1())*bounds.nj()+j);
            qApp->processEvents();
        }

    }

    emit currentTask("Saving result grid");
    emit started(1);
    emit progress(0);
    qApp->processEvents();
    //std::pair<GridType, QString> gridTypeAndName = splitFullGridName( m_gridName );
    if( !project()->addGrid( m_gridType, m_gridName, m_grid, params() )){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_gridName) );
        return ResultCode::Error;
    }
    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}


