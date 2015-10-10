#ifndef SEGYREADER_H
#define SEGYREADER_H

#include<stdexcept>
#include<iostream>
#include<fstream>
#include<cstdint>
#include<memory>

#include<segy_header_def.h>
#include<segy_text_header.h>
#include<segy_binary_header_def.h>
#include<segy_traceheader_def.h>
#include<header.h>
#include<trace.h>


// msvc has no ssize_t!
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif



namespace seismic{



class SEGYReader
{
public:

    using sample_t=float;

    struct FormatError:public std::runtime_error{
        FormatError( const std::string& msg):runtime_error(msg){}
    };

    enum class SampleFormat{ IBM, INT4, INT2, FIXP, IEEE, INT1 };

    static_assert( sizeof(sample_t)==4, "Sample_type must be of 4 bytes size" );

    SEGYReader( const std::string& name,
              bool swap=false,
              size_t raw_binary_header_size=400,
              size_t raw_trace_header_size=240,
              size_t max_samples_per_trace=65535,
              const std::vector<SEGYHeaderWordDef>& binary_header_def=SEGY_DEFAULT_BINARY_HEADER_DEF,
              const std::vector<SEGYHeaderWordDef>& trace_header_def=SEGY_DEFAULT_TRACE_HEADER_DEF);

    const std::vector<SEGYTextHeader>& textHeaders()const{
        return m_text_headers;
    }

    const Header& binaryHeader()const{
        return m_binary_header;
    }

    const std::vector<SEGYHeaderWordDef>& binary_header_def()const{
        return m_binary_header_def;
    }

    const std::vector<SEGYHeaderWordDef>& trace_header_def()const{
        return m_trace_header_def;
    }

    ssize_t current_trace()const{
        return m_cur_trace;
    }

    ssize_t trace_count()const{
        return m_trace_count;
    }

    void set_trace_header_def( const std::vector<SEGYHeaderWordDef>& trace_header_def );


    void set_swap( bool swp ){
        m_swap=swp;
    }

    bool is_swap()const{
        return m_swap;
    }


    bool is_fixed_scalel()const{
        return m_fixed_scalel;
    }

    void set_fixed_scalel( bool on ){
        m_fixed_scalel=on;
    }

    double scalel()const{
        return m_scalel;
    }

    // used as multiplier only, i.e. divisor is number between 0 and 1 instead of negative
    void set_scalel( double s){
        m_scalel=s;
    }


    bool is_fixed_scalco()const{
        return m_fixed_scalco;
    }

    void set_fixed_scalco( bool on ){
        m_fixed_scalco=on;
    }

    double scalco()const{
        return m_scalco;
    }

    // used as multiplier only, i.e. divisor is number between 0 and 1 instead of negative
    void set_scalco( double s){
        m_scalco=s;
    }

    bool is_overide_sample_format()const{
        return m_override_sample_format;
    }

    SampleFormat sample_format()const{
        return m_sample_format;
    }

    void set_sample_format( SampleFormat fmt){
        m_override_sample_format=true;
        m_sample_format=fmt;
    }


    void seek_trace(const ssize_t& n);
    Header read_trace_header();
    Trace read_trace();




private:

    ssize_t bytes_per_trace()const{
        return m_raw_trace_header_buf.size() + m_nt * m_bytes_per_sample;
    }

    void read_samples();

    Header convert_raw_header( std::vector<char>& rhdr, const std::vector<SEGYHeaderWordDef>& def );
    void convert_binary_header();
    void convert_trace_header();
    void convert_raw_samples(Trace& dest);

    void estimate_filesize();
    void read_leading_headers();

    std::ifstream                		m_is;
    bool                                m_swap;
    std::vector<char>                   m_raw_binary_header_buf;
    std::vector<char>                   m_raw_trace_header_buf;
    std::vector<char>                   m_raw_samples_buf;
    std::vector<SEGYHeaderWordDef>      m_binary_header_def;
    std::vector<SEGYHeaderWordDef>      m_trace_header_def;
    SEGYHeaderWordDef                       m_scalco_def;
    SEGYHeaderWordDef                       m_scalel_def;
    std::vector<SEGYTextHeader>         m_text_headers;
    Header                              m_binary_header;
    Header                              m_trace_header;
    bool                                m_override_sample_format=false;
    bool                                m_fixed_scalel=false;
    bool                                m_fixed_scalco=false;
    double                             m_scalel=1;
    double                             m_scalco=1;
    ssize_t                             m_cur_trace=0;
    ssize_t                             m_filesize=0;
    ssize_t                             m_trace_count=0;
    ssize_t                             m_first_trace_pos;
    SampleFormat                        m_sample_format;
    size_t                              m_bytes_per_sample;
    bool                                m_is_next_trace_header;        // is next read a trace header
    bool                                m_fixed_sampling=true;         // if true sampling will not be updated from trace headers, seek is only implemented with true yet!
    Trace::time_type                    m_dt=0;
    size_t                              m_nt=0;
    //Trace                               m_trc;
};




}


#endif // SEGYREADER_H
