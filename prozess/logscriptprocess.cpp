


#include "logscriptprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

#include<cstdio>

extern "C" {
#include<Python.h>
}

namespace Python{
PyObject* mainModule;       // NEED UGLY GLOBAL, failed to create pyobject class member!!!
PyObject* transformFunc;
};

LogScriptProcess::LogScriptProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Log Script"), project, parent){

}

ProjectProcess::ResultCode LogScriptProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);



    try{

        m_outputName=getParam(parameters, "log");
        m_unit=getParam(parameters, "unit");
        m_descr=getParam(parameters, "description");
        m_script=getParam(parameters, "script");

        auto ninputs=getParam(parameters, "n_input_logs").toInt();
        for( int i=0; i<ninputs; i++){
            QString key=QString("input_log_%1").arg(i+1);
            auto name=getParam(parameters, key);
            m_inputNames.push_back(name);
        }

        int i=0;
        while(1){
           QString name=QString("well-%1").arg(++i);
           if( parameters.contains(name)){
               auto well=parameters.value(name);
               m_wells.push_back(well);
           }
           else{
               break;
           }
        }
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode LogScriptProcess::initWell(QString well){

    //load input logs
    m_inputLogs.clear();
    for( auto lname : m_inputNames){
        auto log=project()->loadLog(well, lname);
        if( !log ){
            setErrorString(QString("Loading log \"%1 - %2\" failed!").arg(well, lname));
            return ResultCode::Error;
        }
        m_inputLogs.push_back(log);
    }

    if( m_inputLogs.size()<1){
        setErrorString("Need at least 1 input log");
        return ResultCode::Error;
    }

    // test if all input logs have same number of samples
    for( int i=1; i<m_inputLogs.size(); i++){
        if( m_inputLogs[0]->nz() != m_inputLogs[i]->nz()){
            setErrorString("Not all logs have the same number of samples");
            return ResultCode::Error;
        }
    }

    // allocate outut log
    m_log=std::make_shared<Log>(m_outputName.toStdString(), m_unit.toStdString(), m_descr.toStdString(),
                                m_inputLogs[0]->z0(), m_inputLogs[0]->dz(), m_inputLogs[0]->nz());
    if( !m_log ){
        setErrorString("Allocating log failed!");
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode LogScriptProcess::initPython(){

    // need this on windows
#ifdef _WIN32
    char PythonHome[]="Python27";
    Py_SetPythonHome(PythonHome);
#endif
    // init
    Py_Initialize();


    Python::mainModule = PyImport_AddModule("__main__");
   if( Python::mainModule==NULL){
       PyErr_Print();
       setErrorString("Adding main module failed!");
       return ResultCode::Error;
   }

   // redirect stdout/stderr
   std::string stdOutErr =
"import sys\n"
"class CatchOutErr:\n"
"  def __init__(self):\n"
"    self.value = ''\n"
"  def write(self, txt):\n"
"    self.value += txt\n"
"catchOutErr = CatchOutErr()\n"
"sys.stdout = catchOutErr\n"
"sys.stderr = catchOutErr\n"; //this is python code to redirect stdouts/stderr

   int result=PyRun_SimpleString(stdOutErr.c_str());
   if( result == -1 ){
       PyErr_Print();
       setErrorString("Error running output redirection script");
       return ResultCode::Error;
   }

   PyRun_SimpleString(m_script.toStdString().c_str());

   Python::transformFunc = PyObject_GetAttrString(Python::mainModule, "transform");
   if( !Python::transformFunc ){
        setErrorString("Retrieving transform func failed!");
        return ResultCode::Error;
   }

   return ResultCode::Ok;
}

void LogScriptProcess::finishPython(){

    // cleanup
    Py_DECREF(Python::transformFunc);
    Py_Finalize();
}

ProjectProcess::ResultCode LogScriptProcess::processWell(QString well){


   try{


    // finally run script / iterate

    // add common variables for execution envronment
    // define environment before running the script - vars can be accessed on startup

    auto mainModule=Python::mainModule;

    PyObject* NULL_VALUE=PyFloat_FromDouble(m_log->NULL_VALUE);
    if( NULL_VALUE==NULL || -1==PyObject_SetAttrString(mainModule,"NULL", NULL_VALUE) ){
            throw std::runtime_error("Adding execution environment variable failed!");
    }

    PyObject* dz=PyFloat_FromDouble(m_log->dz());
    if( dz==NULL || -1==PyObject_SetAttrString(mainModule,"DZ", dz) ){
            throw std::runtime_error("Adding execution environment variable failed!");
    }

    PyObject* nz=PyInt_FromLong(m_log->nz());
    if( nz==NULL || -1==PyObject_SetAttrString(mainModule,"NZ", nz) ){
            throw std::runtime_error("Adding execution environment variable failed!");
    }


    for( int i=0; i<m_log->nz(); i++){


        // build argument tuple
        PyObject* argsTuple=PyTuple_New( m_inputLogs.size());
        if( !argsTuple ){
            throw std::runtime_error("creating function argument tuple failed!");
        }

        for( int k=0; k<m_inputLogs.size(); k++){

            double in=(*m_inputLogs[k])[i];
            // in python use infinty as NULL_VALUE
            if( in ==m_inputLogs[k]->NULL_VALUE ){
                in=std::numeric_limits<double>::infinity();
            }
            PyObject* p_in=PyFloat_FromDouble(in);
            if( p_in==NULL){
                throw std::runtime_error("creating function argument double object failed!");
            }

            // Set_Item steals the reference, no decref required
            if( PyTuple_SetItem(argsTuple, k, p_in )!=0 ){
                throw std::runtime_error("adding value to argument tuple failed!");
            }

        }


        // Invoke the function, passing the argument tuple.
        PyObject* result = PyObject_Call(Python::transformFunc, argsTuple, NULL);
        if( !result ){
            throw std::runtime_error("invoking function failed!");
        }

       // Convert the result to a double
       double resultDouble(PyFloat_AsDouble(result));

       // Free all temporary Python objects.
        Py_DECREF(argsTuple);
        Py_DECREF(result);

        // correct different NULL_VALUE
        if( resultDouble==std::numeric_limits<double>::infinity()){
            resultDouble=m_log->NULL_VALUE;
        }

        // assign function resultto result grid
        (*m_log)[i]=resultDouble;
    }

    }
    catch( std::exception& ex){

        PyErr_Print();

        PyObject *catcher = PyObject_GetAttrString(Python::mainModule,"catchOutErr"); //get our catchOutErr created above

        if( !catcher ){
            setErrorString("Accessing catcher failed!");
            return ResultCode::Error;
        }

        PyObject *output = PyObject_GetAttrString(catcher,"value"); //get the stdout and stderr from our catchOutErr object
        if( !output ){
            setErrorString("Accessing output failed!");
            return ResultCode::Error;
        }

        setErrorString(PyString_AsString(output));
        //std::cerr<<PyString_AsString(output)<<std::flush; //it's not in our C++ portion

        Py_Finalize();


        return ResultCode::Error;
    }


    if( !project()->addLog( well, m_log->name().c_str(), *m_log) ){
        setErrorString( QString("Could not add log \"%1\" to project!").arg(m_log->name().c_str()) );
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}


ProjectProcess::ResultCode LogScriptProcess::run(){

    try{

    auto res=initPython();
    if( res!=ResultCode::Ok) return res;

    emit started(m_wells.size());
    int n=0;

    for( auto well : m_wells){

        initWell(well);

        emit currentTask(tr("Processing Well \"%1\"").arg(well));
        qApp->processEvents();

        auto res=processWell(well);
        if( res!=ResultCode::Ok) return res;

        n++;
        emit progress(n);
        qApp->processEvents();
    }

    emit finished();
    qApp->processEvents();

    finishPython();

    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
