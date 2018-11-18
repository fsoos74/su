#ifndef SEGYREADER_H
#define SEGYREADER_H

/*
#include<iostream>
#include<fstream>
*/
#include<cstdio>
#include<cstdint>
#include<memory>
#include<functional>

#include<segy_header_def.h>
#include<segy_text_header.h>
#include<segy_binary_header_def.h>
#include<segy_traceheader_def.h>
#include<header.h>
#include<trace.h>
#include<gather.h>
#include<segyinfo.h>
#include<segysampleformat.h>
#include<segyformaterror.h>

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


    static_assert( sizeof(sample_t)==4, "Sample_type must be of 4 bytes size" );

    SEGYReader( const std::string& name,
                const SEGYInfo& info,
                size_t raw_binary_header_size=400,
                size_t raw_trace_header_size=240,
                size_t max_samples_per_trace=65535);

    const std::vector<SEGYTextHeader>& textHeaders()const{
        return m_text_headers;
    }

    const Header& binaryHeader()const{
        return m_binary_header;
    }


    ssize_t current_trace()const{
        return m_cur_trace;
    }

    ssize_t current_position()const{
        return m_current_position;
    }

    ssize_t trace_count()const{
        return m_trace_count;
    }

    const SEGYInfo& info()const{
        return m_info;
    }

    void setInfo( const SEGYInfo&);

    Trace::time_type dt()const{
        return m_dt;
    }

    size_t nt()const{
        return m_nt;
    }

    void seek_trace(const ssize_t& n);
    Header read_trace_header();
    Trace read_trace();
    std::shared_ptr<Gather> read_gather( const std::string& key, const size_t max_traces=1000);

    // this was added for header byte scans
    const std::vector<char>& rawTraceHeader()const{
        return m_raw_trace_header_buf;
    }

    // switch this to crypt::decrypt when in demo mode
    static std::function<void(void*,size_t)> postReadFunc;

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

    //std::ifstream                		m_is;
    FILE*                               m_ifile;
    SEGYInfo                            m_info;

    std::function<void(void*, size_t)>  m_postReadFunc;

    std::vector<char>                   m_raw_binary_header_buf;
    std::vector<char>                   m_raw_trace_header_buf;
    std::vector<char>                   m_raw_samples_buf;
    std::vector<SEGYTextHeader>         m_text_headers;
    Header                              m_binary_header;
    Header                              m_trace_header;

    ssize_t                             m_cur_trace=0;
    ssize_t                             m_filesize=0;
    ssize_t                             m_trace_count=0;
    ssize_t                             m_first_trace_pos;
    ssize_t                             m_current_position=0;

    size_t                              m_bytes_per_sample;
    bool                                m_is_next_trace_header;        // is next read a trace header
    bool                                m_fixed_sampling=true;         // if true sampling will not be updated from trace headers, seek is only implemented with true yet!
    int                                 m_next=0;						// number of extended ebcdic headers:w

    Trace::time_type                    m_dt=0;
    size_t                              m_nt=0;
    double                              m_ft=0;
    //Trace                               m_trc;
};





}


#endif // SEGYREADER_H
