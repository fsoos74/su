#include "segysampleformat.h"

#include<stdexcept>

namespace seismic{


const QString strIBM="IBM";
const QString strIEEE="IEEE";
const QString strINT1="INT1";
const QString strINT2="INT2";
const QString strINT4="INT4";
const QString strFIXP="FIXP";



QString toQString( SEGYSampleFormat f ){

    if( !SampleFormatInfo.contains(f)){
        throw std::runtime_error("Invalid sample format in toQString( SEGYSampleFormat f )");
    }

    return SampleFormatInfo.value(f).name;
}

SEGYSampleFormat toSampleFormat( const QString& s){

    for( SEGYSampleFormat format : SampleFormatInfo.keys() ){
        if( SampleFormatInfo.value(format).name==s){
            return format;
        }
    }

    throw std::runtime_error("Invalid name in toSampleFormat( const QString& s)");
}


std::int16_t toInt(SEGYSampleFormat f){

    if( !SampleFormatInfo.contains(f)){
        throw std::runtime_error("Invalid sample format in toInt(SEGYSampleFormat f)");
    }

    return SampleFormatInfo.value(f).code;
}

SEGYSampleFormat toSampleFormat(int16_t i){

    for( SEGYSampleFormat format : SampleFormatInfo.keys() ){
        if( SampleFormatInfo.value(format).code==i){
            return format;
        }
    }

    throw std::runtime_error("Invalid code in toSampleFormat( int16_t)");
}


size_t bytesPerSample( SEGYSampleFormat f){

    if( !SampleFormatInfo.contains(f)){
        throw std::runtime_error("Invalid sample format in bytesPerSample( SEGYSampleFormat f)");
    }

    return SampleFormatInfo.value(f).size;
}
}

