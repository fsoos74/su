#include "volumemathprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include<iostream>
#include<QApplication>


VolumeMathProcess::VolumeMathProcess( AVOProject* project, QObject* parent) :
    VolumesProcess( QString("Volume Math"), project, parent){

}

ProjectProcess::ResultCode VolumeMathProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    QString func;
    QString oname;
    QString iname1;
    QString iname2;
    double value=0;
    QString topName;
    QString bottomName;

    try{
        func=getParam(parameters, "function");
        oname=getParam(parameters, "output-volume");
        iname1=getParam(parameters, "input-volume1");
        iname2=getParam(parameters, "input-volume2");
        value=getParam(parameters, "value").toDouble();
        topName=getParam( parameters, "top-horizon");
        bottomName=getParam( parameters, "bottom-horizon");
        m_editMode=static_cast<bool>(getParam(parameters, "edit-mode").toInt());
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    ProjectProcess::ResultCode res=addInputVolume(iname1);
    if(res!=ResultCode::Ok) return res;

    if( !iname2.isEmpty()){
        res=addInputVolume(iname2);
        if(res!=ResultCode::Ok) return res;
        if(inputBounds(0)!=inputBounds(1)){
            setErrorString("Input volumes have different bounds!");
            return ResultCode::Error;
        }
    }

    setBounds(inputBounds(0));

    res=addOutputVolume(oname, bounds(), inputDomain(0), inputType(0));
    if( res!=ResultCode::Ok) return res;

    emit currentTask("Loading Horizons");
    emit started(2);
    qApp->processEvents();
    if(topName!="NONE"){
        m_topHorizon=project()->loadGrid(GridType::Horizon, topName);
        if(!m_topHorizon){
            setErrorString(QString("Loading horizon \"%1\" failed!").arg(topName));
            return ResultCode::Error;
        }
    }
    emit progress(1);
    qApp->processEvents();

    if(bottomName!="NONE"){
        m_bottomHorizon=project()->loadGrid(GridType::Horizon, bottomName);
        if(!m_bottomHorizon){
            setErrorString(QString("Loading horizon \"%1\" failed!").arg(bottomName));
            return ResultCode::Error;
        }
    }
    emit progress(2);
    qApp->processEvents();

    m_processor.setOP(MathProcessor::toOP(func));
    m_processor.setValue(value);

    return ResultCode::Ok;
}

#include<iostream>
ProjectProcess::ResultCode VolumeMathProcess::processInline(
        QVector<std::shared_ptr<Volume> > outputs, QVector<std::shared_ptr<Volume> > inputs, int iline){

    std::cout<<"processInline ni="<<inputs.size()<<" no="<<outputs.size()<<" iline="<<iline<<std::endl<<std::flush;
    std::shared_ptr<Volume> input1=inputs[0];
    std::shared_ptr<Volume> input2;
    if( inputs.size()>1) input2=inputs[1];      // only used when op needs 2 input values
    std::shared_ptr<Volume> output=outputs[0];

    if( m_editMode){
        for( int j=bounds().j1(); j<=bounds().j2(); j++){
            for( int k=0; k<bounds().nt(); k++){
                (*output)(iline,j,k)=(*input1)(iline,j,k);
            }
        }
    }

    m_processor.setInputNullValue(input1->NULL_VALUE);
    int k1=0;
    int k2=bounds().nt();

    for( int j=bounds().j1(); j<=bounds().j2(); j++){

        if( m_topHorizon ){
            auto z = m_topHorizon->valueAt(iline,j);
            if( z==m_topHorizon->NULL_VALUE) continue;      // skip cdp without top horizon
            k1=std::max( 0, static_cast<int>( std::round( ( 0.001*z -bounds().ft() ) / bounds().dt() ) ) );        // maybe use this as time to sample in bounds
        }
        if( m_bottomHorizon ){
            auto z = m_bottomHorizon->valueAt(iline,j);
            if( z==m_bottomHorizon->NULL_VALUE) continue;      // skip cdp without bottom horizon
            k2=std::min( bounds().nt(), static_cast<int>( std::round( ( 0.001*z -bounds().ft() ) / bounds().dt() ) ) );
        }
        for( int k=k1; k<k2; k++){

            auto ivalue1=(*input1)(iline,j,k);
            m_processor.setInput1(ivalue1);

            if(input2){
                auto ivalue2=(*input2)(iline,j,k);
                m_processor.setInput2(ivalue2);
            }

            auto res=m_processor.compute();
            if( res==m_processor.NULL_VALUE){
                res=output->NULL_VALUE;
            }

            (*output)(iline,j,k)=res;
        }
    }

    return ResultCode::Ok;
}
