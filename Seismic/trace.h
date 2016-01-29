#ifndef TRACE_H
#define TRACE_H



#include <vector>
#include <cmath>
#include "header.h"

namespace seismic{

class Trace{

public:
    using sample_type=float;
    using Samples=std::vector<sample_type>;
	using size_type=size_t;
	using time_type=double;

	Trace( time_type ft=0, time_type dt=-1, size_type nt=0 ):m_ft(ft), m_dt(dt), m_samples(nt){
	}

    Trace( time_type ft, time_type dt, size_type nt, const Header& hdr):m_ft(ft), m_dt(dt), m_samples(nt), m_header(hdr){
    }

    Trace( time_type ft, time_type dt, const Samples& sam, const Header& hdr):m_ft(ft), m_dt(dt), m_samples(sam), m_header(hdr){
	}

	time_type ft()const{
		return m_ft;
	}

	time_type dt()const{
		return m_dt;
	}

	time_type lt()const{
		return (size()>0) ? m_ft + (size()-1)*m_dt : m_ft;
	}

	size_type size()const{
		return m_samples.size();
	}

	const Samples& samples()const{
		return m_samples;
	}

	Samples& samples(){
		return m_samples;
	}

    const Header& header()const{
        return m_header;
	}

    Header& header(){
        return m_header;
	}

	sample_type operator[](size_type i)const{
		check_index(i);
		return m_samples[i];
	}

	sample_type& operator[](size_type i){
		check_index(i);
		return m_samples[i];
	}

	size_type time_to_index( time_type t ) const{
		if( t<m_ft ) return size();
		size_type idx = static_cast<size_type>( std::trunc( (t-m_ft)/m_dt) );
		return idx<size() ? idx : size();
	}



private:
	void check_index(size_type i)const{
		assert( i<m_samples.size() );
	}

	time_type m_ft;
	time_type m_dt;
	Samples m_samples;
    Header m_header;
};

}

#endif

