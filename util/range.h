#ifndef RANGE_H
#define RANGE_H

template<typename T>
struct Range{

    Range():minimum(0),maximum(0){};
    Range( T min, T max):minimum(min), maximum(max){}

    bool isInside(T t)const{
        return (t>=minimum && t<=maximum);
    }

    bool operator==(const Range& other)const{
        return (minimum==other.minimum) && (maximum==other.maximum);
    }

    bool operator!=(const Range& other)const{
        return !((*this)==other);
    }


    T minimum;
    T maximum;
};

#endif // RANGE_H
