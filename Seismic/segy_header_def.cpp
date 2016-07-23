#include "segy_header_def.h"

#include<stdexcept>

namespace seismic{


const QString strINT16="INT16";
const QString strINT32="INT32";
const QString strUINT16="UINT16";
const QString strBUF6="BUF66";
const QString strUINT8="UINT8";
const QString strIEEE="IEEE";

const QString strPLAIN="PLAIN";
const QString strFLOAT="FLOAT";
const QString strCOORD="COORD";
const QString strELEV="ELEV";


QString toQString(SEGYHeaderWordDataType t){

    switch( t ){

    case SEGYHeaderWordDataType::INT16: return strINT16; break;

    case SEGYHeaderWordDataType::INT32: return strINT32; break;

    case SEGYHeaderWordDataType::UINT16: return strUINT16; break;

    case SEGYHeaderWordDataType::BUF6: return strBUF6; break;

    case SEGYHeaderWordDataType::UINT8: return strUINT8; break;

    case SEGYHeaderWordDataType::IEEE: return strIEEE; break;

    default: throw std::runtime_error("ILLEGAL data type in toQString(SEGYHeaderWordDataType)");
    }

}

SEGYHeaderWordDataType toDataType(const QString& s){

    if( s==strINT16){
        return SEGYHeaderWordDataType::INT16;
    }
    else if( s==strINT32){
        return SEGYHeaderWordDataType::INT32;
    }
    else if( s==strUINT16){
        return SEGYHeaderWordDataType::UINT16;
    }
    else if( s==strBUF6){
        return SEGYHeaderWordDataType::BUF6;
    }
    else if( s==strUINT8){
        return SEGYHeaderWordDataType::UINT8;
    }
    else if( s==strIEEE){
        return SEGYHeaderWordDataType::IEEE;
    }

    throw std::runtime_error("ILLEGAL type in toDataType(QString)");
}


QString toQString(SEGYHeaderWordConvType t){

    switch( t ){

    case SEGYHeaderWordConvType::PLAIN: return strPLAIN; break;

    case SEGYHeaderWordConvType::FLOAT: return strFLOAT; break;

    case SEGYHeaderWordConvType::COORD: return strCOORD; break;

    case SEGYHeaderWordConvType::ELEV: return strELEV; break;

    default: throw std::runtime_error("ILLEGAL type in toQString(SEGYHeaderWordConvType)!");
    }
}

SEGYHeaderWordConvType toConvType(const QString& s){

    if( s == strPLAIN){
        return SEGYHeaderWordConvType::PLAIN;
    }
    else if( s == strFLOAT){
        return SEGYHeaderWordConvType::FLOAT;
    }
    else if( s == strCOORD){
        return SEGYHeaderWordConvType::COORD;
    }
    else if( s == strELEV){
        return SEGYHeaderWordConvType::ELEV;
    }

    throw std::runtime_error("ILLEGAL type in toConvType(QString)!");
}


}
