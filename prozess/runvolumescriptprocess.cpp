
extern "C" {
#include<Python.h>
}

#include "runvolumescriptprocess.h"

#include <avoproject.h>
#include "utilities.h"

#include"linearregression.h"

#include<QVector>
#include<QPointF>
#include<QObject>
#include<QThread>

#include<cstdio>

RunVolumeScriptProcess::RunVolumeScriptProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Run Volume Script"), project, parent){

}

ProjectProcess::ResultCode RunVolumeScriptProcess::init( const QMap<QString, QString>& parameters ){


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
    Grid2DBounds bounds2(bounds.inline1(), bounds.crossline1(),
                         bounds.inline2(), bounds.crossline2());
    for( int i=0; i<m_inputGrid.size(); i++){
        if( !(m_inputGrid[i]->bounds()==bounds2) ){
            setErrorString(QString::asprintf(
                 "Geometry of grid %d differs from first volume", i+1));
            return ResultCode::Error;
        }
    }

    m_volume=std::shared_ptr<Grid3D<float> >( new Grid3D<float>(bounds));


    if( !m_volume){
        setErrorString("Allocating volume failed!");
        return ResultCode::Error;
    }



    std::cout<<"Run Volume Process Params:"<<std::endl;
    std::cout<<"result: "<<m_volumeName.toStdString()<<std::endl;
    for(int i=0; i<m_inputVolumeName.size(); i++){
        std::cout<<"input volume "<<i+1<<": "<<m_inputVolumeName[i].toStdString()<<std::endl;
    }
    for(int i=0; i<m_inputGridName.size(); i++){
        std::cout<<"input grid "<<i+1<<": "<<m_inputGridName[i].toStdString()<<std::endl;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode RunVolumeScriptProcess::run(){


    // need this on windows
#ifdef _WIN32
    char PythonHome[]="Python27";
    Py_SetPythonHome(PythonHome);
#endif
    // init
    Py_Initialize();



    PyObject* mainModule = PyImport_AddModule("__main__");
   if( mainModule==NULL){
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



   try{

   Grid3DBounds bounds=m_volume->bounds();

   // add common variables for execution envronment
   // define environment before running the script - vars can be accessed on startup
   PyObject* iline1=PyInt_FromLong(bounds.inline1());
   if( iline1==NULL || -1==PyObject_SetAttrString(mainModule,"ILINE1", iline1) ){
           throw std::runtime_error("Adding execution environment variable failed!");
   }

   PyObject* iline2=PyInt_FromLong(bounds.inline2());
   if( iline2==NULL || -1==PyObject_SetAttrString(mainModule,"ILINE2", iline2) ){
           throw std::runtime_error("Adding execution environment variable failed!");
   }

   PyObject* xline1=PyInt_FromLong(bounds.crossline1());
   if( xline1==NULL || -1==PyObject_SetAttrString(mainModule,"XLINE1", xline1) ){
           throw std::runtime_error("Adding execution environment variable failed!");
   }

   PyObject* xline2=PyInt_FromLong(bounds.crossline2());
   if( xline2==NULL || -1==PyObject_SetAttrString(mainModule,"XLINE2", xline2) ){
           throw std::runtime_error("Adding execution environment variable failed!");
   }

   PyObject* ft=PyFloat_FromDouble(bounds.ft());
   if( ft==NULL || -1==PyObject_SetAttrString(mainModule,"FT", ft) ){
           throw std::runtime_error("Adding execution environment variable failed!");
   }

   PyObject* dt=PyFloat_FromDouble(bounds.dt());
   if( dt==NULL || -1==PyObject_SetAttrString(mainModule,"DT", dt) ){
           throw std::runtime_error("Adding execution environment variable failed!");
   }

   PyObject* nt=PyInt_FromLong(bounds.sampleCount());
   if( nt==NULL || -1==PyObject_SetAttrString(mainModule,"NT", nt) ){
           throw std::runtime_error("Adding execution environment variable failed!");
   }

   PyRun_SimpleString(m_script.toStdString().c_str());

   PyObject* transformFunc = PyObject_GetAttrString(mainModule, "transform");
   if( !transformFunc ){
     throw std::runtime_error("retrieving transform func failed!");
   }

    // iterate over all cdps and samples

    emit currentTask("Iterating cdps");
    emit started(bounds.inlineCount());
    qApp->processEvents();


    for( int i=bounds.inline1(); i<=bounds.inline2(); i++){

        // add inline as variable for script execution
        PyObject* iline=PyInt_FromLong(i);
        if( iline==NULL || -1==PyObject_SetAttrString(mainModule,"ILINE", iline) ){
                throw std::runtime_error("Adding execution environment variable failed!");
        }

        for( int j=bounds.crossline1(); j<=bounds.crossline2(); j++){

            // add crossline as variable for script execution
            PyObject* xline=PyInt_FromLong(j);
            if( xline==NULL || -1==PyObject_SetAttrString(mainModule,"XLINE", xline) ){
                    throw std::runtime_error("Adding execution environment variable failed!");
            }



            for( int k=0; k<bounds.sampleCount(); k++){

                // add sample time as variable for script execution
                double t=bounds.sampleToTime(k);
                PyObject* time=PyFloat_FromDouble(t);
                if( time==NULL || -1==PyObject_SetAttrString(mainModule,"TIME", time) ){
                        throw std::runtime_error("Adding execution environment variable failed!");
                }


                // build argument tuple
                PyObject* argsTuple=PyTuple_New( m_inputVolume.size()+ m_inputGrid.size());
                if( !argsTuple ){
                    throw std::runtime_error("creating function argument tuple failed!");
                }

                //std::cout<<i<<" "<<j<<" "<<k<<std::endl<<std::flush;
                int ii=0;

                for( int vi=0; vi<m_inputVolume.size(); vi++, ii++){

                    double in=(*m_inputVolume[vi])(i,j,k);
                    // in python use infinty as NULL_VALUE
                    if( in ==m_inputVolume[vi]->NULL_VALUE ){
                        in=std::numeric_limits<double>::infinity();
                    }
                    PyObject* p_in=PyFloat_FromDouble(in);
                    if( p_in==NULL){
                        throw std::runtime_error("creating function argument double object failed!");
                    }

                    // Set_Item steals the reference, no decref required
                    if( PyTuple_SetItem(argsTuple, ii, p_in )!=0 ){
                        throw std::runtime_error("adding value to argument tuple failed!");
                    }

                }

                for( int gi=0; gi<m_inputGrid.size(); gi++, ii++){

                    double in=(*m_inputGrid[gi])(i,j);
                    // in python use infinty as NULL_VALUE
                    if( in ==m_inputGrid[gi]->NULL_VALUE ){
                        in=std::numeric_limits<double>::infinity();
                    }
                    PyObject* p_in=PyFloat_FromDouble(in);
                    if( p_in==NULL){
                        throw std::runtime_error("creating function argument double object failed!");
                    }

                    // Set_Item steals the reference, no decref required
                    if( PyTuple_SetItem(argsTuple, ii, p_in )!=0 ){
                        throw std::runtime_error("adding value to argument tuple failed!");
                    }

                }


                // Invoke the function, passing the argument tuple.
                PyObject* result = PyObject_Call(transformFunc, argsTuple, NULL);
                if( !result ){
                    throw std::runtime_error("invoking function failed!");
                }

               // Convert the result to a double
               double resultDouble(PyFloat_AsDouble(result));

               // Free all temporary Python objects.
                Py_DECREF(time);
                Py_DECREF(argsTuple);
                Py_DECREF(result);

                // correct different NULL_VALUE
                if( resultDouble==std::numeric_limits<double>::infinity()){
                    resultDouble=m_volume->NULL_VALUE;
                }

                // assign function resultto result grid
                (*m_volume)(i,j,k)=resultDouble;
            }

            Py_DECREF( xline );

        }

        Py_DECREF(iline);

        emit progress(i-bounds.inline1());
        qApp->processEvents();

    }

    // cleanup
    Py_DECREF(transformFunc);

    }
    catch( std::exception& ex){

        PyErr_Print();

        PyObject *catcher = PyObject_GetAttrString(mainModule,"catchOutErr"); //get our catchOutErr created above

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


    Py_Finalize();

    emit currentTask("Saving result volume");
    emit started(1);
    emit progress(0);
    qApp->processEvents();

    if( !project()->addVolume( m_volumeName, m_volume)){
        setErrorString( QString("Could not add volume \"%1\" to project!").arg(m_volumeName) );
        return ResultCode::Error;
    }

    emit progress(1);
    qApp->processEvents();

    emit finished();
    qApp->processEvents();

    return ResultCode::Ok;
}


