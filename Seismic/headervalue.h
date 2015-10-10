#ifndef HEADERVALUE_H
#define HEADERVALUE_H



#include<cassert>
#include<iostream>

namespace seismic{


class HeaderValue
{
friend bool operator<( const HeaderValue& a, const HeaderValue& b);
public:
    using int_type=long;
    using uint_type=unsigned long;
    using float_type=double;
    enum Type{ INT_VALUE, UINT_VALUE, FLOAT_VALUE };

    explicit HeaderValue( int_type i ):m_type(INT_VALUE), m_int_value(i){
    }

    explicit HeaderValue( uint_type u ):m_type(UINT_VALUE), m_uint_value(u){
    }

    explicit HeaderValue( float_type f ):m_type(FLOAT_VALUE), m_float_value(f){
    }

    HeaderValue():HeaderValue(int_type(0)){
    }

    Type type()const{
        return m_type;
    }

    int_type intValue()const{
        check( INT_VALUE );
        return m_int_value;
    }

    int_type& intValue(){
        check( INT_VALUE );
        return m_int_value;
    }

    uint_type uintValue()const{
        check( UINT_VALUE );
        return m_uint_value;
    }

    uint_type& uintValue(){
        check( UINT_VALUE );
        return m_uint_value;
    }

    float_type floatValue()const{
        check( FLOAT_VALUE );
        return m_float_value;
    }

    float_type& floatValue(){
        check( FLOAT_VALUE );
        return m_float_value;
    }

    float_type toFloat()const{
        switch( m_type ){
            case Type::INT_VALUE: return m_int_value;break;
            case Type::UINT_VALUE: return m_uint_value; break;
            case Type::FLOAT_VALUE: return m_float_value; break;
            default: throw std::runtime_error("Invalid Type of headervalue!"); break;
        }
    }

private:
    void check( Type t )const{
        assert( t==m_type);
    }

    Type m_type;
    union{
        int_type    m_int_value;
	uint_type   m_uint_value;
        float_type  m_float_value;
    };
};


inline std::ostream& operator<<(std::ostream& os, const HeaderValue& hv){

	switch( hv.type() ){
	case HeaderValue::INT_VALUE:
		os<<hv.intValue()<<"(i)";
		break;
	case HeaderValue::UINT_VALUE:
		os<<hv.uintValue()<<"(u)";
		break;
	case HeaderValue::FLOAT_VALUE:
		os<<hv.floatValue()<<"(f)";
		break;
	default:
		assert( "invalid header value type" == nullptr );
		break;
	}

	return os;
}


inline bool operator<( const HeaderValue& a, const HeaderValue& b){
    // compare as floats
    return a.toFloat() < b.toFloat();
}

}

#endif // HEADERVALUE_H

