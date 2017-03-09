#include "cropgridprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include<iostream>

CropGridProcess::CropGridProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Crop Grid"), project, parent){

}

ProjectProcess::ResultCode CropGridProcess::init( const QMap<QString, QString>& parameters ){

    if( !parameters.contains(QString("grid-type"))){
        setErrorString("Parameters contain no grid type!");
        return ResultCode::Error;
    }
    m_gridType = toGridType(parameters.value(QString("grid-type") ) );

    if( !parameters.contains(QString("output-name"))){
        setErrorString("Parameters contain no output grid!");
        return ResultCode::Error;
    }
    m_outputName=parameters.value(QString("output-name"));

    if( !parameters.contains(QString("input-name"))){
        setErrorString("Parameters contain no input grid!");
        return ResultCode::Error;
    }
    m_inputName=parameters.value(QString("input-name"));

    if( !parameters.contains(QString("min-iline"))){

        setErrorString("Parameters contain no minimum inline!");
        return ResultCode::Error;
    }
    int minIline=parameters.value( QString("min-iline") ).toInt();

    if( !parameters.contains(QString("max-iline"))){

        setErrorString("Parameters contain no maximum inline!");
        return ResultCode::Error;
    }
    int maxIline=parameters.value( QString("max-iline") ).toInt();

    if( !parameters.contains(QString("min-xline"))){

        setErrorString("Parameters contain no minimum crossline!");
        return ResultCode::Error;
    }
    int minXline=parameters.value( QString("min-xline") ).toInt();

    if( !parameters.contains(QString("max-xline"))){

        setErrorString("Parameters contain no maximum crossline!");
        return ResultCode::Error;
    }
    int maxXline=parameters.value( QString("max-xline") ).toInt();


    //load input grid
    m_inputGrid=project()->loadGrid( m_gridType, m_inputName);
    if( !m_inputGrid ){
        setErrorString("Loading grid failed!");
        return ResultCode::Error;
    }

    // compute output grid bounds
    Grid2DBounds bounds=m_inputGrid->bounds();

    int il1=std::max(bounds.i1(), minIline);
    int il2=std::min(bounds.i2(), maxIline);
    if( il1>il2 ){
        setErrorString("Invalid output inline range!");
        return ResultCode::Error;
    }

    int xl1=std::max(bounds.j1(), minXline);
    int xl2=std::min(bounds.j2(), maxXline);
    if( xl1>xl2 ){
        setErrorString("Invalid output crossline range!");
        return ResultCode::Error;
    }

    // creatge output grid
    Grid2DBounds obounds(il1, xl1, il2, xl2);

    m_outputGrid=std::shared_ptr<Grid2D<float> >( new Grid2D<float>(obounds));

    if( !m_outputGrid){
        setErrorString("Allocating grid failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode CropGridProcess::run(){


    Grid2DBounds bounds=m_outputGrid->bounds();

    int onePercent=(bounds.i2()-bounds.i1()+1)/100 + 1; // adding one to avoids possible division by zero

    emit currentTask("Computing output grid");
    emit started(100);
    qApp->processEvents();

    for( int i=bounds.i1(); i<=bounds.i2(); i++){

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            (*m_outputGrid)(i,j)=(*m_inputGrid)( i, j );

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

    if( !project()->addGrid( m_gridType, m_outputName, m_outputGrid)){
        setErrorString( QString("Could not add grid \"%1\" to project!").arg(m_outputName) );
        return ResultCode::Error;
    }

    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();


    return ResultCode::Ok;
}
