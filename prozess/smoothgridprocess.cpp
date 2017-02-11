#include "smoothgridprocess.h"

#include <avoproject.h>

SmoothGridProcess::SmoothGridProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Smooth Grid"), project, parent){

}

ProjectProcess::ResultCode SmoothGridProcess::init( const QMap<QString, QString>& parameters ){


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

    if( !parameters.contains(QString("half-ilines"))){
        setErrorString("Parameters contain no half-ilines");
        return ResultCode::Error;
    }
    m_halfIlines=parameters.value(QString("half-ilines") ).toInt();

    if( !parameters.contains(QString("half-xlines"))){
        setErrorString("Parameters contain no half-xlines");
        return ResultCode::Error;
    }
    m_halfXlines=parameters.value(QString("half-xlines") ).toInt();


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

        int imin=std::max(bounds.i1(), i-m_halfIlines );
        int imax=std::min(bounds.i2(), i+m_halfIlines);

        for( int j=bounds.j1(); j<=bounds.j2(); j++){

            int jmin=std::max(bounds.j1(), j-m_halfXlines );
            int jmax=std::min(bounds.j2(), j+m_halfXlines);


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
            // no else because outpt is NULL anyway
        }

        emit progress(i-bounds.i1());
        qApp->processEvents();
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


