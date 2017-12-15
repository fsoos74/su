#ifndef VOLUMEDIMENSIONS_H
#define VOLUMEDIMENSIONS_H

#include<algorithm>

struct VolumeDimensions{

    VolumeDimensions( int il1, int il2, int xl1, int xl2, int ms1, int ms2):
        inline1(il1), inline2(il2), crossline1(xl1), crossline2(xl2), msec1(ms1), msec2(ms2){
    }

    VolumeDimensions():VolumeDimensions(0,0,0,0,0,0){

    }

    bool isValid()const{
        return inline1<inline2 && crossline1<crossline2 && msec1<msec2;
    }

    bool isInside(int il, int xl, int ms)const{
        return il>=inline1 && il<=inline2 && xl>=crossline1 && xl<=crossline2 && ms>=msec1 && ms<=msec2;
    }

    int inlineCount()const{
        return inline2-inline1+1;
    }

    int crosslineCount()const{
        return crossline2-crossline1+1;
    }

    int msecCount()const{
        return msec2-msec1+1;
    }

    int inline1;
    int inline2;

    int crossline1;
    int crossline2;

    int msec1;
    int msec2;
};

inline bool operator==(const VolumeDimensions& d1, const VolumeDimensions& d2 ){

    return d1.inline1==d2.inline1 && d1.inline2==d2.inline2 &&
            d1.crossline1==d2.crossline1 && d1.crossline2==d2.crossline2 &&
            d1.msec1==d2.msec1 && d1.msec2==d2.msec2;
}

inline bool operator!=(const VolumeDimensions& d1, const VolumeDimensions& d2 ){
    return !(d1==d2);
}

inline VolumeDimensions operator&(const VolumeDimensions& d1, const VolumeDimensions& d2){

    return VolumeDimensions(
                std::max(d1.inline1, d2.inline1),
                std::min(d1.inline2, d2.inline2),
                std::max(d1.crossline1, d2.crossline1),
                std::min(d1.crossline2, d2.crossline2),
                std::max(d1.msec1, d2.msec1),
                std::min(d1.msec2, d2.msec2)
                );

}

inline VolumeDimensions operator|(const VolumeDimensions& d1, const VolumeDimensions& d2){

    return VolumeDimensions(
                std::min(d1.inline1, d2.inline1),
                std::max(d1.inline2, d2.inline2),
                std::min(d1.crossline1, d2.crossline1),
                std::max(d1.crossline2, d2.crossline2),
                std::min(d1.msec1, d2.msec1),
                std::max(d1.msec2, d2.msec2)
                );

}

#endif // VOLUMEDIMENSIONS_H
