#include "headerui.h"

#include<QString>

QString toQString(const seismic::HeaderValue& hv){

    switch( hv.type() ){
    case seismic::HeaderValue::INT_VALUE:
        return QString::number( hv.intValue() );
        break;
    case seismic::HeaderValue::UINT_VALUE:
        return QString::number( hv.uintValue() );
        break;
    case seismic::HeaderValue::FLOAT_VALUE:
        return QString::number( hv.floatValue() );
        break;
    default:
        qFatal( "invalid header value type == nullptr" );
        break;
    }

}
