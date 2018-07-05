#ifndef _MATRIX_H
#define _MATRIX_H

#include<iostream>
#include<vector>
#include<initializer_list>
#include<functional>
#include<algorithm>

template<typename T>
class Matrix{

public:


    class Iterator{ // : public std::iterator<std::forward_iterator_tag,T>{
    public:

        using value_type = T ;
        using reference = T& ;
        using pointer = T* ;
        using difference_type = std::ptrdiff_t ;
        using iterator_category	= std::forward_iterator_tag ;

        Iterator( T* p=nullptr, size_t step=0 ):m_p(p), m_step(step){
        }

        Iterator operator++(){
            m_p+=m_step;
            return *this;
        }

        Iterator operator++(int){
            Iterator result(*this);
            ++(*this);
            return result;
        }

        T& operator*(){
            return *m_p;
        }

        bool operator==(const Iterator& other){
            return m_p==other.m_p;
        }

        bool operator!=(const Iterator& other){
            return m_p!=other.m_p;
        }

    private:

        T* m_p;
        size_t m_step;
    };


	using iterator = Iterator;//typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;

    Matrix( size_t n=1, size_t m=1, T ival=T(0) ) : m_N(n), m_M(m),m_data(n*m, ival){
	}

    Matrix( size_t n, size_t m, std::initializer_list<T> l):m_N(n), m_M(m), m_data(l){
		if( m_data.size()!=m_N*m_M) throw std::runtime_error("Wrong number of initializer elements!");
	}

	template<class ITER>
    Matrix( size_t n, size_t m, ITER first, ITER last):m_N(n), m_M(m), m_data(first, last){
		if( m_data.size()!=m_N*m_M) throw std::runtime_error("Wrong number of initializer elements!");
	}

    size_t rows()const{
		return m_N;
	}

    size_t columns()const{
		return m_M;
	}

    size_t size()const{
		return m_data.size();
	}

    T& operator[](size_t idx){
		return m_data[idx];
	}

    const T& operator[](size_t idx)const{
		return m_data[idx];
	}

    T& operator()(size_t i, size_t j){
		return m_data[toIndex(i,j)];
	}

    const T& operator()(size_t i, size_t j)const{
		return m_data[toIndex(i,j)];
	}

	iterator begin(){
		return Iterator(&m_data[0], 1 );// return m_data.begin();
	}

	iterator end(){
		return Iterator(&m_data[0]+m_data.size(),0); //m_data.end();
	}

    iterator row_begin(size_t i){
		return Iterator(&m_data[i*m_M], 1 );
	}

    iterator row_end(size_t i){
		return Iterator(&m_data[i*m_M]+m_M,0);
	}

    iterator column_begin(size_t j){
		return Iterator(&m_data[j], m_M );
	}

    iterator column_end(size_t j){
		return Iterator(&m_data[j]+m_N*m_M,0);
	}

	const_iterator cbegin()const{
		return m_data.cbegin();
	}

	const_iterator cend()const{
		return m_data.cend();
	}


	Matrix<T> transposed()const{
		Matrix<T> m(m_M, m_N);
        for( size_t i=0; i<m_N; i++){
            for( size_t j=0; j<m_M; j++){
				m(j,i)=(*this)(i,j);
			}
		}
		return m;
	}

    Matrix<T> row(size_t i)const{
        Matrix<T> m(1,m_M);
        for( size_t j=0; j<m_M; j++){
            m(0,j)=(*this)(i,j);        // optimize with copy
        }
        return m;
    }

    Matrix<T> column(size_t j)const{
        Matrix<T> m(m_N, 1);
        for( size_t i=0; i<m_N; i++){
            m(i,0)=(*this)(i,j);
        }
        return m;
    }

    Matrix<T>& apply( std::function<T(T)> f){
        std::for_each( m_data.begin(), m_data.end(),
            [f](T& t){t=f(t);});
        return *this;
    }

	Matrix<T>& operator+=(const Matrix<T>& other){

        // for other row-vector add columns to all rows
        if( other.rows()==1 && other.columns()==m_M){
            
            for( size_t i=0; i<m_N; i++){
                for( size_t j=0; j<m_M; j++){
                    (*this)(i,j)+=other(0,j);
                }
            }
            return *this;
        }

		if( other.rows()!=m_N || other.columns()!=m_M) 
			throw std::runtime_error("Matrices have different dimensions!");

		auto it1=m_data.begin();
		auto it2=other.cbegin();
		while(it1!=m_data.end()){
			*it1+=*it2;
			++it1;
			++it2;	
		}
		
		return *this;
	}

	Matrix<T>& operator-=(const Matrix<T>& other){

		if( other.rows()!=m_N || other.columns()!=m_M) 
			throw std::runtime_error("Matrices have different dimensions!");

		auto it1=m_data.begin();
		auto it2=other.cbegin();
		while(it1!=m_data.end()){
			*it1-=*it2;
			++it1;
			++it2;	
		}
		
		return *this;
	}

	Matrix<T>& operator*=(const Matrix<T>& other){

		if( other.rows()!=m_N || other.columns()!=m_M) 
			throw std::runtime_error("Matrices have different dimensions!");

		auto it1=m_data.begin();
		auto it2=other.cbegin();
		while(it1!=m_data.end()){
			*it1*=*it2;
			++it1;
			++it2;	
		}
		
		return *this;
	}

	Matrix<T>& operator/=(const Matrix<T>& other){

		if( other.rows()!=m_N || other.columns()!=m_M) 
			throw std::runtime_error("Matrices have different dimensions!");

		auto it1=m_data.begin();
		auto it2=other.cbegin();
		while(it1!=m_data.end()){
			*it1/=*it2;
			++it1;
			++it2;	
		}
		
		return *this;
	}

	Matrix<T>& operator+=(const double& x){
        for( auto &v : m_data ){
            v+=x;
        }
        return *this;
    } 

	Matrix<T>& operator-=(const double& x){
        for( auto &v : m_data ){
            v-=x;
        }
        return *this;
    } 

	Matrix<T>& operator*=(const double& x){
        for( auto &v : m_data ){
            v*=x;
        }
        return *this;
    } 

	Matrix<T>& operator/=(const double& x){
        for( auto &v : m_data ){
            v/=x;
        }
        return *this;
    } 

private:
    size_t toIndex(size_t i, size_t j)const{ return i*m_M+j; }
    std::pair<size_t, size_t> fromIndex( size_t idx )const{
        return std::make_pair( static_cast<size_t>(idx/m_M), static_cast<size_t>(idx%m_M) );
	}

    size_t m_N;
    size_t m_M;
	std::vector<T> m_data;
};

template<typename T>
Matrix<T> operator+(Matrix<T> m1, const Matrix<T>& m2){
	m1+=m2;
	return m1;
}

template<typename T>
Matrix<T> operator-(Matrix<T> m1, const Matrix<T>& m2){
	m1-=m2;
	return m1;
}

template<typename T>
Matrix<T> operator*(Matrix<T> m1, const Matrix<T>& m2){
	m1*=m2;
	return m1;
}

template<typename T>
Matrix<T> operator/(Matrix<T> m1, const Matrix<T>& m2){
	m1/=m2;
	return m1;
}

template<typename T>
Matrix<T> operator+(Matrix<T> m1, const double& x){
	return m1+=x;
}

template<typename T>
Matrix<T> operator+(const double& x,Matrix<T> m1){
	return m1+=x;
}

template<typename T>
Matrix<T> operator-(Matrix<T> m1, const double& x){
	return m1-=x;
}

template<typename T>
Matrix<T> operator*(Matrix<T> m1, const double& x){
	return m1*=x;
}

template<typename T>
Matrix<T> operator*(const double& x, Matrix<T> m1){
	return m1*=x;
}

template<typename T>
Matrix<T> operator/(Matrix<T> m1, const double& x){
	return m1/=x;
}

template<typename T> 
Matrix<T> dot(const Matrix<T>& m1, const Matrix<T>& m2){

	if( m1.columns()!=m2.rows() ) throw std::runtime_error("Matrices have invalid shape!");

	Matrix<T> res(m1.rows(), m2.columns());

    for( size_t i=0; i<res.rows(); i++){

        for( size_t j=0; j<res.columns(); j++){

			T sum=0;
            for( size_t k=0; k<m1.columns(); k++){
				sum+=m1(i,k)*m2(k,j);
			}
			res(i,j)=sum;	
		}
	}

	return res;
}


template<typename T>
Matrix<T> columnAverage( const Matrix<T>& m ){

        Matrix<T> a(1, m.columns());
        for( size_t j=0; j<m.columns(); j++){
            T sum=0;
            for( size_t i=0; i<m.rows(); i++){
                sum+=m(i,j);
            }
            a(0,j)=sum/m.rows();
        }
        return a;
}


template<typename T> 
Matrix<T> addColumn(const Matrix<T>& m, const T& ival=T(0)){

    Matrix<T> a(m.rows(), m.columns()+1, ival );
    for( size_t i=0; i<m.rows(); i++){
        for( size_t j=0; j<m.columns(); j++){
            a(i,j)=m(i,j);      // could use copy for speed 
        }
    }
    return a;
}

template<typename T> 
Matrix<T> removeColumn(const Matrix<T>& m ){

    Matrix<T> a(m.rows(), m.columns()-1 );
    for( size_t i=0; i<m.rows(); i++){
        for( size_t j=0; j+1<m.columns(); j++){
            a(i,j)=m(i,j);      // could use copy for speed 
        }
    }
    return a;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m){

	os<<"[ ";
    for( size_t i=0; i<m.rows(); i++){
		if( i>0 ) os<<","<<std::endl;
		os<<"[ ";
        for( size_t j=0; j<m.columns(); j++){
			if( j>0 ) os<<", ";			
			os<<m(i,j);
		}
		os<<" ]";
	}
	os<<" ]";

	return os;
}

#endif
