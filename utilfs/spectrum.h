#ifndef SPECTRUM_H
#define SPECTRUM_H

#include<vector>
#include<complex>
#include<cmath>

class Spectrum
{
public:

    typedef float float_type;
    typedef std::complex< float_type > value_type;
    typedef std::vector< value_type > data_type;
    typedef size_t index_type;
    typedef data_type::const_iterator const_iterator;

    Spectrum(){}        // all default constructor, len=0

    template< class IT >
    inline Spectrum( float_type f0, float_type df, IT beg, IT end);

    inline index_type size()const;

    inline float_type fmin()const;
    inline float_type fmax()const;
    inline float_type fincr()const;

    inline float_type frequency( index_type )const;
    inline value_type value( index_type )const;
    inline float_type amplitude( index_type )const;
    inline float_type phase( index_type )const;

    inline const_iterator cbegin()const;
    inline const_iterator cend()const;

    inline static bool isNull( const Spectrum&);

protected:

    inline const data_type& data()const;

private:

    float_type m_ff;    // first frequency
    float_type m_df;    // frequency increment
    data_type m_data;   // amplitude and phase for each frequency as complex number

};

inline bool Spectrum::isNull( const Spectrum& s ){ return s.data().empty(); }

template< class IT >
Spectrum::Spectrum( Spectrum::float_type f0, Spectrum::float_type df, IT beg, IT end):
    m_ff(f0), m_df(df), m_data(beg, end ){
}

Spectrum::index_type Spectrum::size()const{
    return m_data.size();
}

Spectrum::float_type Spectrum::fmin()const{
    return m_ff;
}

Spectrum::float_type Spectrum::fmax()const{
    return m_ff + m_df * m_data.size();
}

Spectrum::float_type Spectrum::fincr()const{
    return m_df;
}

const Spectrum::data_type& Spectrum::data()const{
    return m_data;
}

Spectrum::float_type Spectrum::frequency( Spectrum::index_type i)const{
    return m_ff + i*m_df;
}

Spectrum::value_type Spectrum::value( Spectrum::index_type i)const{
    return m_data[i];
}

Spectrum::float_type Spectrum::amplitude( Spectrum::index_type i)const{
    return std::abs(value(i));
}

Spectrum::float_type Spectrum::phase( Spectrum::index_type i)const{
    return std::arg(value(i));
}

Spectrum::const_iterator Spectrum::cbegin()const{
    return m_data.cbegin();
}

Spectrum::const_iterator Spectrum::cend()const{
    return m_data.cend();
}

#endif // SPECTRUM_H
