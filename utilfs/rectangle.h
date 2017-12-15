#ifndef RECTANGLE_H
#define RECTANGLE_H


namespace util{

template<typename T>
class Rectangle{

public:
    Rectangle()=default;
    Rectangle(const Rectangle&)=default;

    Rectangle( const T& x1, const T& y1, const T& x2, const T& y2)
        :m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2){
        normalize();
    }

    T x1()const{
        return m_x1;
    }

    T y1()const{
        return m_y1;
    }

    T x2()const{
        return m_x2;
    }

    T y2()const{
        return m_y2;
    }

    T width()const{
        return m_x2-m_x1+1;
    }

    T height()const{
        return m_y2-m_y1+1;
    }

    bool isInside( const T& x, const T& y)const{
        return x>=m_x1 && x<=m_x2 && y>=m_y1 && y<=m_y2;
    }

private:

    // gurantee x1<x2 and y1<y2
    void normalize(){
        if(m_x2<m_x1) std::swap(m_x1, m_x2);
        if(m_y2<m_y1) std::swap(m_y1, m_y2);
    }

    T m_x1;
    T m_y1;
    T m_x2;
    T m_y2;
};

}

#endif // RECTANGLE_H

