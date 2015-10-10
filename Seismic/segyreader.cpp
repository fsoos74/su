#include "segyreader.h"
#include<seis_bits.h>


#include<fstream>
#include<iomanip>

namespace seismic{

void convert_ibm_to_samples( const std::vector<char>& src, Trace::Samples& dest, size_t n, bool chend=false);

Header SEGYReader::convert_raw_header( std::vector<char>& rhdr, const std::vector<SEGYHeaderWordDef>& defs ){

    Header th;


    for( auto def : defs ){

        switch(def.dtype){
        case SEGY_INT16:{
            std::int16_t i;
            get_from_raw( &i, &rhdr[0] + def.pos -1, m_swap );
            if( def.ctype==SEGY_ELEV ){
                th[def.name]=HeaderValue(HeaderValue::float_type( i*m_scalel ));
            }
            else if( def.ctype==SEGY_COORD ){
                th[def.name]=HeaderValue(HeaderValue::float_type( i*m_scalco ) );
            }
            else{
                th[def.name]=HeaderValue(HeaderValue::int_type( i ) );
            }
            break;
        }
        case SEGY_INT32:{
            std::int32_t i;
            get_from_raw( &i, &rhdr[0] + def.pos -1, m_swap );
            if( def.ctype==SEGY_ELEV ){
                th[def.name]=HeaderValue( HeaderValue::float_type( i*m_scalel ) );
            }
            else if( def.ctype==SEGY_COORD ){
                th[def.name]=HeaderValue(HeaderValue::float_type( i*m_scalco ) );
            }
            else{
                th[def.name]=HeaderValue(HeaderValue::int_type( i ) );
            }
            break;
        }
        case SEGY_UINT16:{
            std::uint16_t u;
            get_from_raw( &u, &rhdr[0] + def.pos - 1, m_swap );
            if( def.ctype==SEGY_PLAIN ){
                th[def.name]=HeaderValue(HeaderValue::uint_type( u ) );
            }
            else{
                throw FormatError("SEGY_UINT32 values can only be type plain!");
            }
            break;
        }
        case SEGY_BUF6:
             // ignore for now!!!!
            break;
        /*
        case SEGY_FLOAT:{
            float f;
            get_from_raw( &f, &rhdr[def.pos], m_swap );
            if( def.ctype==SEGY_PLAIN ){
                th[def.name]=HeaderValue(HeaderValue::float_type( f ) );
            }
            else{
                throw FormatError("SEGY_FLOAT values can only be type plain!");
            }
            break;
        }*/
        default:
            throw FormatError("Unsupported type in convert_raw_header!");
            break;
        } // switch


    } // for

    return th;
}


SEGYReader::SEGYReader( const std::string& name,
          bool swap,
          size_t raw_binary_header_size,
          size_t raw_trace_header_size,
          size_t max_samples_per_trace,
            const std::vector<SEGYHeaderWordDef>& binary_header_def,
            const std::vector<SEGYHeaderWordDef>& trace_header_def
          ):
        m_is(name, std::ios::in|std::ios::binary),
        m_swap(swap),
        m_raw_binary_header_buf(raw_binary_header_size),
        m_raw_trace_header_buf(raw_trace_header_size),
        m_raw_samples_buf( max_samples_per_trace * sizeof(sample_t)),
        m_binary_header_def( binary_header_def),
        m_trace_header_def(trace_header_def),
        m_scalco_def("scalel", SEGY_PLAIN, SEGY_INT16,  69, R"( Scalar for elevations and depths (+ = multiplier, - = divisor)
                                                              )"),
        m_scalel_def("scalco", SEGY_PLAIN, SEGY_INT16,  71, R"( Scalar for coordinates (+ = multiplier, - = divisor)
                                                              )")
{
    if( !m_is.good()) throw( FormatError("Open file failed!"));
    estimate_filesize();
    read_leading_headers();
    m_trace_count=( m_filesize - m_first_trace_pos ) / bytes_per_trace();
}


void SEGYReader::set_trace_header_def( const std::vector<SEGYHeaderWordDef>& def ){
    m_trace_header_def=def;
}


void SEGYReader::convert_binary_header(){

    m_binary_header=convert_raw_header(m_raw_binary_header_buf, m_binary_header_def);

    if( !m_override_sample_format){
        switch( m_binary_header["format"].intValue()){
            case 1: m_sample_format=SampleFormat::IBM; m_bytes_per_sample=4;break;
            case 2: m_sample_format=SampleFormat::INT4; m_bytes_per_sample=4;break;
            case 3: m_sample_format=SampleFormat::INT2; m_bytes_per_sample=2;break;
            case 4: m_sample_format=SampleFormat::FIXP; m_bytes_per_sample=4;break;
            case 5: m_sample_format=SampleFormat::IEEE; m_bytes_per_sample=4;break;
            case 8: m_sample_format=SampleFormat::INT1; m_bytes_per_sample=1;break;
            default: throw FormatError("Unsupported sample format code!");break;
        }
    }

    m_nt=m_binary_header["ns"].uintValue();
}

void SEGYReader::convert_trace_header(){

    if( !m_fixed_scalel ){
        std::int16_t iscalel;
        get_from_raw( &iscalel, &m_raw_trace_header_buf[0] + m_scalel_def.pos, m_swap );
        m_scalel=( iscalel<0 ) ? 1./static_cast<double>(-iscalel) : iscalel;
    }

    if( !m_fixed_scalco ){
        std::int16_t iscalco;
        get_from_raw( &iscalco, &m_raw_trace_header_buf[0] + m_scalco_def.pos, m_swap );
        m_scalco=( iscalco<0 ) ? 1./static_cast<double>(-iscalco) : iscalco;
    }

    m_trace_header=convert_raw_header(m_raw_trace_header_buf, m_trace_header_def);

    if( !m_fixed_sampling ){
        m_dt=0.000001 * m_trace_header["dt"].intValue();
        m_nt=m_trace_header["ns"].uintValue();
    }
}

void SEGYReader::estimate_filesize(){
    m_is.seekg(0,m_is.end);
    m_filesize=m_is.tellg();
    m_is.seekg(0,m_is.beg);
}

void SEGYReader::read_leading_headers(){

    // read mandatory first text/ebcdic header
    SEGYTextHeader text_hdr;
    m_is.read(&text_hdr[0], text_hdr.size());
    if( !m_is.good()) throw(FormatError("Reading mandatory ebcdic header failed!"));
    m_text_headers.push_back(text_hdr);

    // read binary header
    m_is.read(&m_raw_binary_header_buf[0], m_raw_binary_header_buf.size());
    if( !m_is.good()) throw(FormatError("Reading binary header failed!"));
    convert_binary_header();

    // read optional text headers as specified in binary header
    int next=m_binary_header["next"].intValue();
    if( next<0 ) throw FormatError("variable number of extended textual headers is not implemented yet!");
    for( int i=0; i<next; i++){
        m_is.read(&text_hdr[0], text_hdr.size());
        if( !m_is.good()) throw(FormatError("Reading mandatory ebcdic header failed!"));
        m_text_headers.push_back(text_hdr);
    }

    // store beginning of traces
    m_first_trace_pos=m_is.tellg();
    m_cur_trace=0;
    m_is_next_trace_header=true;
}


void SEGYReader::seek_trace(const ssize_t& n){

    if( n>=m_trace_count ){
        throw FormatError("Trying to access trace after eof!");
    }
    ssize_t pos=m_first_trace_pos + n * bytes_per_trace() ;
    m_is.seekg( pos, m_is.beg);
    m_cur_trace=n;
    m_is_next_trace_header=true;
}


Header SEGYReader::read_trace_header(){

    if(!m_is_next_trace_header){
        throw( FormatError("Attempting to read trace header from illegal position!"));
    }


    m_is.read(&m_raw_trace_header_buf[0], m_raw_trace_header_buf.size());
    if( !m_is.good()) throw(FormatError("Reading trace header failed!"));
    convert_trace_header();

    m_is_next_trace_header=false;

    return m_trace_header;
}


void SEGYReader::read_samples(){

    if(m_is_next_trace_header){
        throw( FormatError("Attempting to read samples from illegal position!"));
    }

    ssize_t nbytes=m_bytes_per_sample * m_nt;
    m_is.read( &m_raw_samples_buf[0], nbytes );
    if( !m_is.good() ) throw FormatError("Reading samples failed!");
}

void SEGYReader::convert_raw_samples( Trace& trc){
    switch( m_sample_format ){
    case SampleFormat::IBM: convert_ibm_to_samples( m_raw_samples_buf, trc.samples(), m_nt);break;
    /*
    case 2: m_sample_format=SampleFormat::INT4; m_bytes_per_sample=4;break;
    case 3: m_sample_format=SampleFormat::INT2; m_bytes_per_sample=2;break;
    case 4: m_sample_format=SampleFormat::FIXP; m_bytes_per_sample=4;break;
    case 5: m_sample_format=SampleFormat::IEEE; m_bytes_per_sample=4;break;
    case 8: m_sample_format=SampleFormat::INT1; m_bytes_per_sample=1;break;
    */
    default: throw FormatError("Unsupported sample format code!");break;
    }
}

Trace SEGYReader::read_trace(){

    if(m_is_next_trace_header ){
        read_trace_header();
    }

    Trace trc(0, m_dt, m_nt );      // sampling params m_dt and m_nt are updated in convert_trace_header

    read_samples();
    convert_raw_samples(trc);

    m_is_next_trace_header=true;

    m_cur_trace++;

    return trc;
}


void convert_ibm_to_samples( const std::vector<char>& src, Trace::Samples& dest, size_t n, bool chend){

    int32_t fconv, fmant, t;

    for ( size_t i=0; i<n; i++)
    {
            fconv = ((int32_t*)&src[0])[i];

            if (!chend)
                    fconv = (fconv<<24) | ((fconv>>24)&0xff) | ((fconv&0xff00)<<8) | ((fconv&0xff0000)>>8);

            if (fconv)
            {
                    fmant = 0x00ffffff & fconv;


                    t = (int32_t) ((0x7f000000 & fconv) >> 22) - 130;

                    while (!(fmant & 0x00800000)) { --t; fmant <<= 1; }


                    if (t > 254)
                            fconv = (0x80000000 & fconv) | 0x7f7fffff;
                    else if (t <= 0)
                            fconv = 0;
                    else
                            fconv = (0x80000000 & fconv) |(t << 23)|(0x007fffff & fmant);
            }

            dest[i] = *((float*)&(fconv));

    }

}


}
