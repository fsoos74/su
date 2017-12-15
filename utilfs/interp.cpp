#include "interp.h"


//namespace util{

double lininterp(double x1, double y1, double x2, double y2, double x){

    if( x<=x1 ) return y1;
    if( x>=x2 ) return y2;
    if( x1 == x2 ) return (y1+y2)/2;

    // need to add eps check

    return y1 + x * ( y2 - y1 ) / ( x2 - x1 );
}


//}
