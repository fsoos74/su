#include "volumecircledetectionprocess.h"

#include <avoproject.h>
#include "utilities.h"
#include <processparams.h>
#include <QApplication>
#include<iostream>
#include<cstring>
#include <matrix.h>
#include <grid2d.h>
#include <array2d.h>

namespace{


Grid2D<float> convolve( const Grid2D<float>& src, Matrix<float> kernel){

    auto bounds=src.bounds();
    Grid2D<float> dest(bounds);

    for( int i=bounds.i1()+kernel.rows()/2; i<=bounds.i2()-kernel.rows()/2; i++){

        for( int j=bounds.j1()+kernel.columns()/2; j<=bounds.j2()-kernel.columns()/2; j++){

            qreal sv=0;

            for( int ii=0; ii<kernel.rows(); ii++){

                for( int jj=0; jj<kernel.columns(); jj++){

                    auto ciijj=kernel(ii,jj);
                    auto v=src(i+ii,j+jj);
                    if( v==src.NULL_VALUE) continue;
                    sv+=ciijj*v;
                }

            }

            dest(i,j)=sv;
        }
    }

    return dest;
}

/*

Grid2D<float> houghCircle( const Grid2D<float>& g, int r){

    auto b2d=g.bounds();

    auto arr=g.values();
    util::Array2D<int> acc(arr.rows()+2*r, arr.columns()+2*r,0);

    Matrix<float>

            For each pixel(x,y)
              For each radius r = 10 to r = 60 // the possible radius
                For each theta t = 0 to 360  // the possible  theta 0 to 360
                   a = x – r * cos(t * PI / 180); //polar coordinate for center
                   b = y – r * sin(t * PI / 180);  //polar coordinate for center
                   A[a,b,r] +=1; //voting
                end
              end
            end



}
*/

}

VolumeCircleDetectionProcess::VolumeCircleDetectionProcess( AVOProject* project, QObject* parent) :
    ProjectProcess( QString("Volume Circle Detection"), project, parent){

}

ProjectProcess::ResultCode VolumeCircleDetectionProcess::init( const QMap<QString, QString>& parameters ){

    setParams(parameters);

    try{
        m_outputName=getParam(parameters, "output-volume");
        m_inputName=getParam(parameters, "input-volume");
        m_minRadius=getParam(parameters, "min-radius").toInt();
        m_maxRadius=getParam(parameters, "max-radius").toInt();
    }
    catch(std::exception& ex){
        setErrorString(ex.what());
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}

ProjectProcess::ResultCode VolumeCircleDetectionProcess::run(){

    auto reader=project()->openVolumeReader(m_inputName);

    if( !reader || !reader->good()){
        setErrorString("Open volume reader failed!");
        return ResultCode::Error;
    }

    auto bounds=reader->bounds();

    auto writer=project()->openVolumeWriter(m_outputName, bounds, reader->domain(), reader->type());

    if( !writer ){
        setErrorString("Open volume writer failed!");
        return ResultCode::Error;
    }

    if( !writer->good()){
        setErrorString(writer->lastError());
        return ResultCode::Error;
    }

    Matrix<float> kernel(3,3,-1.);
    kernel(1,1)=8.;
    const int CHUNK_SAMPLES=10;

    emit started(bounds.nt());
    qApp->processEvents();

    for( int k0=0; k0<bounds.nt(); k0+=CHUNK_SAMPLES){

        auto subvolume=reader->readK(k0, std::min(k0+CHUNK_SAMPLES-1, bounds.nt()-1));
        if( !subvolume){
            setErrorString(QString("Reading subvolume failed: %1").arg(reader->lastError()));
            writer->removeFile();
            return ResultCode::Error;
        }

        // iterate over constant sample number slices in subvolume
        for( int k=0; k<subvolume->bounds().nt(); k++){

            //extract slice
            auto kslice=subvolume->atK(k);

            // first step edge detection
            auto edges=convolve(kslice, kernel);

            // put back slice
            subvolume->setK(k,edges);
        }

        if(!writer->writeK(*subvolume)){
            setErrorString(QString("Writing subvolume failed: %1").arg(writer->lastError()));
            writer->removeFile();
            return ResultCode::Error;
        }

        emit progress(k0);
        qApp->processEvents();
    }

    reader->close();
    writer->close();

    if( !project()->addVolume( m_outputName, params() ) ){
        writer->removeFile();
        return ResultCode::Error;
    }

    return ResultCode::Ok;
}
