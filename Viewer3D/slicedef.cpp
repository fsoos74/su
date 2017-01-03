#include "slicedef.h"

const QString INLINE_STRING("Inline");
const QString CROSSLINE_STRING("Crossline");
const QString SAMPLE_STRING("Sample");

QString toQString(SliceType t){

    switch(t){
    case SliceType::INLINE: return INLINE_STRING;break;
    case SliceType::CROSSLINE: return CROSSLINE_STRING;break;
    case SliceType::SAMPLE: return SAMPLE_STRING;break;
    default: qFatal("Invalid SliceType");
    }
}

SliceType toSliceType(QString str){

    if( str==INLINE_STRING) return SliceType::INLINE;
    else if(str==CROSSLINE_STRING) return SliceType::CROSSLINE;
    else if( str==SAMPLE_STRING) return SliceType::SAMPLE;
    else qFatal("Invalid SliceType string");

}
