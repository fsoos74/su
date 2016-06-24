#include "segywriter.h"
#include<seis_bits.h>


#include<fstream>
#include<iomanip>

#if defined(_MSC_VER)

#define fseek _fseeki64
#define ftell _ftelli64

#endif

namespace seismic{

void convert_ibm_samples( char* rawbuf, const Trace::Samples& samples, size_t n, bool swap);
void convert_ieee_samples( char* rawbuf, const Trace::Samples& samples, size_t n, bool swap );
void convert_int4_samples( char* rawbuf, const Trace::Samples& samples, size_t n, bool swap);
void convert_int2_samples( char*  rawbuf, const Trace::Samples& samples, size_t n, bool swap);
void convert_int1_samples( char*  rawbuf, const Trace::Samples& samples, size_t n, bool swap);
void convert_fixp_samples( char*  rawbuf, const Trace::Samples& samples, size_t n, bool swap);


void SEGYWriter::convert_header( std::vector<char>& rhdr, const Header& header, const std::vector<SEGYHeaderWordDef>& defs ){

    for( auto def : defs ){

        // skip non existent headerword
        Header::const_iterator it=header.find(def.name);
        if( it==header.end()) continue;

        const HeaderValue& value=it->second;

        switch(def.dtype){

        case SEGYHeaderWordDataType::INT16:{
            std::int16_t i;

            if( def.ctype==SEGYHeaderWordConvType::ELEV ){
                i=static_cast<std::int16_t>(std::round( value.floatValue()/m_info.scalel()) );
            }
            else if( def.ctype==SEGYHeaderWordConvType::COORD ){
                i=static_cast<std::int16_t>(std::round( value.floatValue()/m_info.scalco()) );
            }
            else{
                i=static_cast<std::int16_t>(value.intValue());
            }

            put_to_raw( &i, &rhdr[0] + def.pos -1, m_info.isSwap() );
            break;
        }
        case SEGYHeaderWordDataType::INT32:{
            std::int32_t i=12345;

            if( def.ctype==SEGYHeaderWordConvType::ELEV ){
                i=static_cast<std::int32_t>(std::round( value.floatValue()/m_info.scalel()) );
            }
            else if( def.ctype==SEGYHeaderWordConvType::COORD ){
                i=static_cast<std::int32_t>(std::round( value.floatValue()/m_info.scalco()) );
            }
            else{
                i=static_cast<std::int32_t>(value.intValue());
            }

            put_to_raw( &i, &rhdr[0] + def.pos -1, m_info.isSwap() );
            break;
        }
        case SEGYHeaderWordDataType::UINT8:{
            std::uint8_t u;

            if( def.ctype==SEGYHeaderWordConvType::PLAIN ){
                u=static_cast<std::uint8_t>(value.uintValue());
            }
            else{
                throw FormatError("SEGY_UINT8 values can only be type plain!");
            }

            put_to_raw( &u, &rhdr[0] + def.pos -1 );
            break;
        }
        case SEGYHeaderWordDataType::UINT16:{
            std::uint16_t u;

            if( def.ctype==SEGYHeaderWordConvType::PLAIN ){
                u=static_cast<std::uint16_t>(value.uintValue());
            }
            else{
                throw FormatError("SEGY_UINT16 values can only be type plain!");
            }

            put_to_raw( &u, &rhdr[0] + def.pos -1, m_info.isSwap() );
            break;
        }
        case SEGYHeaderWordDataType::BUF6:
             // ignore for now!!!!
            break;
        case SEGYHeaderWordDataType::IEEE:{
            float f;

            if( def.ctype==SEGYHeaderWordConvType::PLAIN ){
                f=static_cast<float>(value.floatValue());
            }
            else{
                throw FormatError("SEGY_IEEE values can only be type plain!");
            }

            put_to_raw( &f, &rhdr[0] + def.pos -1, m_info.isSwap() );
            break;
        }
        default:
            throw FormatError("Unsupported type in convert_header!");
            break;
        } // switch


    } // for

}


SEGYWriter::SEGYWriter( const std::string& name,
          const SEGYInfo& info,
          const SEGYTextHeader& textHeader,   //only support 1 ebcdic/textual header block for now
          const Header& binaryHeader,
          size_t raw_binary_header_size,
          size_t raw_trace_header_size,
          size_t max_samples_per_trace
          ):
        m_ofile(fopen(name.c_str(), "wb")),
        m_info(info),
        m_text_header(textHeader),
        m_binary_header(binaryHeader),
        m_raw_binary_header_buf(raw_binary_header_size),
        m_raw_trace_header_buf(raw_trace_header_size),
        m_raw_samples_buf( max_samples_per_trace * sizeof(sample_t))
{
    std::cout<<"SEGYWriter::SEGYWriter"<<std::endl<<std::flush;

    if( !m_ofile) throw( FormatError("Open file failed!"));

    m_bytes_per_sample=bytesPerSample(m_info.sampleFormat());
    m_binary_header["format"]=HeaderValue::makeIntValue( toInt(m_info.sampleFormat()) );

    // write_leading_headers(); // dont call from here because virtual functions

}

SEGYWriter::~SEGYWriter(){
    std::fflush(m_ofile);
    fclose(m_ofile);
}


void SEGYWriter::setInfo( const SEGYInfo& info ){
    m_info=info;
}


void SEGYWriter::convert_binary_header(){

    convert_header(m_raw_binary_header_buf, m_binary_header, m_info.binaryHeaderDef());
}

void SEGYWriter::convert_trace_header(){

   convert_header(m_raw_trace_header_buf, m_trace_header, m_info.traceHeaderDef());
}

//just a dummy for standard segy
void SEGYWriter::process_raw_binary_header(std::vector<char> &){
    std::cout<<"SEGYWriter::process_raw_binary_header(std::vector<char> &)"<<std::endl<<std::flush;
}

void SEGYWriter::write_leading_headers(){

    if(fwrite(&m_text_header, m_text_header.size(), 1, m_ofile)!=1){
        throw(FormatError("Writing mandatory ebcdic header failed!"));
    }

    // convert and write binary header

    convert_binary_header();
    process_raw_binary_header( m_raw_binary_header_buf ); // make additional modifications required by non standar segy

    if(fwrite(&m_raw_binary_header_buf[0], m_raw_binary_header_buf.size(), 1, m_ofile)!=1){
        throw(FormatError("Writing binary header failed!"));
    }

    m_cur_trace=0;
}

void SEGYWriter::write_trace_header(){

    if(fwrite(&m_raw_trace_header_buf[0], m_raw_trace_header_buf.size(), 1, m_ofile)!=1){
        throw(FormatError("Writing trace header failed!"));
    }

}


void SEGYWriter::write_samples(size_t nt){

    ssize_t nbytes=m_bytes_per_sample * nt;
    if( fwrite( &m_raw_samples_buf[0], nbytes, 1, m_ofile )!=1){
        throw FormatError("Writing samples failed!");
    }
}

void SEGYWriter::convert_samples( const Trace& trc){
    switch( m_info.sampleFormat() ){
    //case SEGYSampleFormat::IBM: convert_ibm_samples( &m_raw_samples_buf[0], trc.samples(), trc.size(), m_info.isSwap());
    //    break;
    case SEGYSampleFormat::IEEE: convert_ieee_samples( &m_raw_samples_buf[0], trc.samples(), trc.size(), m_info.isSwap() );
        break;
    //case SEGYSampleFormat::INT4: convert_int4_samples( &m_raw_samples_buf[0], trc.samples(), trc.size(), m_info.isSwap());
    //    break;
    //case SEGYSampleFormat::INT2: convert_int2_samples( &m_raw_samples_buf[0], trc.samples(), trc.size(), m_info.isSwap());
    //    break;
    //case SEGYSampleFormat::INT1: convert_int1_samples( &m_raw_samples_buf[0], trc.samples(), trc.size(), m_info.isSwap());
    //    break;
    //case SEGYSampleFormat::FIXP: convert_fixp_samples( &m_raw_samples_buf[0], trc.samples(), trc.size(), m_info.isSwap());
    //    break;
    default: throw FormatError("Unsupported sample format code!");break;
    }
}

void SEGYWriter::write_trace(const Trace& trace){

    // replace copies?
    m_trace_header=trace.header();

    // write start time as recording delay
    m_trace_header["delrt"]=HeaderValue::makeIntValue(1000*trace.ft()); // XXX

    convert_trace_header();
    write_trace_header();

    convert_samples(trace);
    write_samples(trace.size());

    m_cur_trace++;
}


void SEGYWriter::write_gather( const Gather& gather){

    for( const Trace& trace : gather ){
        write_trace(trace);
    }

}


void convert_ibm_samples( char* rawbuf, const Trace::Samples& samples, size_t n, bool swap){
    throw std::runtime_error("Not implemented; convert_ibm_samples!");
}

void convert_ieee_samples(char* rawbuf, const Trace::Samples& samples, size_t n, bool swap ){

    char* dest=rawbuf;
    for( size_t i=0; i<n; i++){

        put_to_raw((int32_t*)(&samples[i]), dest, swap);
        dest+=4;
    }
}

void convert_int4_samples( char* rawbuf, const Trace::Samples& samples, size_t n, bool swap){
    throw std::runtime_error("Not implemented; convert_int4_samples!");
}

void convert_int2_samples( char*  rawbuf, const Trace::Samples& samples, size_t n, bool swap){
    throw std::runtime_error("Not implemented; convert_int2_samples!");
}

void convert_int1_samples( char*  rawbuf, const Trace::Samples& samples, size_t n, bool swap){
    throw std::runtime_error("Not implemented; convert_int1_samples!");
}

void convert_fixp_samples( char*  rawbuf, const Trace::Samples& samples, size_t n, bool swap){
    throw std::runtime_error("Not implemented; convert_fixp_samples!");
}




}
