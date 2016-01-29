#include <QCoreApplication>

#include<grid3d.h>
#include<volumewriter.h>
#include<volumereader.h>
#include<iostream>
#include<fstream>
#include<iomanip>


void fillVolume(Grid3D<float>& vol){

    long x=0;
    Grid3DBounds bounds=vol.bounds();
    for( int iline=bounds.inline1(); iline<=bounds.inline2(); iline++){
        for( int xline=bounds.crossline1(); xline<=bounds.crossline2(); xline++){
            for( int sample=0; sample<bounds.sampleCount(); sample++){
                vol(iline, xline, sample)=x++;
            }
        }
    }
}

bool compareVolumes( const Grid3D<float>& v1, const Grid3D<float>& v2){

    Grid3DBounds bounds=v1.bounds();
    if(v2.bounds()!=bounds) return false;

    for( int iline=bounds.inline1(); iline<=bounds.inline2(); iline++){
        for( int xline=bounds.crossline1(); xline<=bounds.crossline2(); xline++){
            for( int sample=0; sample<bounds.sampleCount(); sample++){
                if( v1(iline, xline, sample)!=v2(iline, xline, sample) ) return false;
            }
        }
    }

    return true;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Grid3DBounds bounds(1,160, 1,260, 1000, 0, 1);
    Grid3D<float> volume(bounds);
    fillVolume(volume);
    const char* filename="/home/fsoos/testvolume.bin";

    VolumeWriter writer(volume);
    std::ofstream ostr(filename, std::ios::out|std::ios::binary);
    if( !writer.writeToStream(ostr)){
        std::cout<<"Write failed: "<<writer.lastError().toStdString()<<std::endl;
        return -1;
    }
    ostr.close();


    Grid3D<float> volume2;
    VolumeReader reader(volume2);
    std::ifstream istr(filename, std::ios::in|std::ios::binary);
    if( !reader.readFromStream(istr)){
        std::cout<<"Read failed: "<<reader.lastError().toStdString()<<std::endl;
        return -1;
    }
    istr.close();

    if(!compareVolumes(volume, volume2)){
        std::cout<<"Volumes are different!!!!"<<std::endl;
        return -2;
    }

    std::cout<<"FULL SUCCESS!"<<std::endl;

    return a.exec();
}
