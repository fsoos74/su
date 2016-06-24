#ifndef SEGYWRITER_H
#define SEGYWRITER_H

#include<stdexcept>
/*
#include<iostream>
#include<fstream>
*/
#include<cstdio>
#include<cstdint>
#include<memory>

#include<segy_header_def.h>
#include<segy_text_header.h>
#include<segy_binary_header_def.h>
#include<segy_traceheader_def.h>
#include<header.h>
#include<trace.h>
#include<gather.h>
#include<segyinfo.h>
#include<segysampleformat.h>

#include<functional>

// msvc has no ssize_t!
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif



namespace seismic{



class SEGYWriter
{
public:

    using sample_t=float;

    struct FormatError:public std::runtime_error{
        FormatError( const std::string& msg):runtime_error(msg){}
    };


    static_assert( sizeof(sample_t)==4, "Sample_type must be of 4 bytes size" );

    SEGYWriter( const std::string& name,
                const SEGYInfo& info,
                const SEGYTextHeader& textHeader,   //only support 1 ebcdic/textual header block for now
                const Header& binaryHeader,
                size_t raw_binary_header_size=400,
              size_t raw_trace_header_size=240,
              size_t max_samples_per_trace=65535);

    ~SEGYWriter();


    const SEGYTextHeader& textHeader()const{
        return m_text_header;
    }

    const Header& binaryHeader()const{
        return m_binary_header;
    }


    ssize_t current_trace()const{
        return m_cur_trace;
    }

    ssize_t trace_count()const{
        return m_trace_count;
    }

    const SEGYInfo& info()const{
        return m_info;
    }

    void setInfo( const SEGYInfo&);

    void write_leading_headers();
    void write_trace(const Trace&);
    void write_gather(const Gather&);

protected:
    virtual void process_raw_binary_header( std::vector<char>&);

private:

    void write_trace_header();
    void write_samples(size_t);

    void convert_header( std::vector<char>& rhdr, const Header& header, const std::vector<SEGYHeaderWordDef>& def );
    void convert_binary_header();
    void convert_trace_header();
    void convert_samples(const Trace& source);



    FILE*                               m_ofile;
    SEGYInfo                            m_info;


    std::vector<char>                   m_raw_binary_header_buf;
    std::vector<char>                   m_raw_trace_header_buf;
    std::vector<char>                   m_raw_samples_buf;
    SEGYTextHeader                      m_text_header;
    Header                              m_binary_header;
    Header                              m_trace_header;

    ssize_t                             m_cur_trace=0;
    ssize_t                             m_trace_count=0;

    size_t                              m_bytes_per_sample;

};




}


#endif // SEGYWRITER_H
