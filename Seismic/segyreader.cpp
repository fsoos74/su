#include "segyreader.h"
#include<seis_bits.h>


#include<fstream>
#include<iomanip>

#if defined(_MSC_VER)

#define fseek _fseeki64
#define ftell _ftelli64

#endif

namespace seismic{

std::function<void(void*,size_t)> SEGYReader::postReadFunc=[](void*, size_t){return;};            // default do nothing


void convert_ibm_to_samples(  char* src, Trace::Samples& dest, size_t n, bool chend);
void convert_ieee_to_samples( char* src, Trace::Samples& dest, size_t n, bool chend);
void convert_int4_to_samples( char* src, Trace::Samples& dest, size_t n, bool chend);
void convert_int2_to_samples( char* src, Trace::Samples& dest, size_t n, bool chend);
void convert_int1_to_samples( char* src, Trace::Samples& dest, size_t n, bool chend);
void convert_fixp_to_samples( char* src, Trace::Samples& dest, size_t n, bool chend);

Header SEGYReader::convert_raw_header( std::vector<char>& rhdr, const std::vector<SEGYHeaderWordDef>& defs ){

    Header th;

    // XXX
    double coordfac=1.;
    double elevfac=1.;

    for( auto def : defs ){

        switch(def.dtype){
        case SEGYHeaderWordDataType::INT16:{
            std::int16_t i;
            get_from_raw( &i, &rhdr[0] + def.pos -1, m_info.isSwap() );
            th[def.name]=HeaderValue(HeaderValue::int_type( i ) );
            break;
        }
        case SEGYHeaderWordDataType::INT32:{
            std::int32_t i;
            get_from_raw( &i, &rhdr[0] + def.pos -1, m_info.isSwap() );
            th[def.name]=HeaderValue(HeaderValue::int_type( i ) );
            break;
        }
        case SEGYHeaderWordDataType::UINT16:{
            std::uint16_t u;
            get_from_raw( &u, &rhdr[0] + def.pos - 1, m_info.isSwap() );
            if( def.ctype==SEGYHeaderWordConvType::PLAIN ){
                th[def.name]=HeaderValue(HeaderValue::uint_type( u ) );
            }
            else{
                throw SEGYFormatError("SEGY_UINT32 values can only be type plain!");
            }
            break;
        }
        case SEGYHeaderWordDataType::BUF6:
             // ignore for now!!!!
            break;

        case SEGYHeaderWordDataType::IEEE:{
            float f;
            get_from_raw( &f, &rhdr[0] + def.pos - 1 , m_info.isSwap() );  // pos is 1-based !!!
            th[def.name]=HeaderValue(HeaderValue::float_type( f ) );
            break;
        }
        default:
            throw SEGYFormatError("Unsupported type in convert_raw_header!");
            break;
        } // switch


    } // for

    // get coordinate scaler if needed
    if( m_info.isFixedScalco() ){
        coordfac=m_info.scalco();   // this is a factor already
    }else{
        try{
        auto it=th.find("scalco");
        if( it!=th.end() ){
            auto h = th["scalco"].intValue();
            if( h<0 ){
                coordfac = -1./h;
            }
            else if( h>0 ){
                coordfac = h;
            }
        }
        }catch(...){
            coordfac=1.;
        }

    }

    // apply coordinate scaler where needed
    for( auto def : defs ){

        if( def.ctype==SEGYHeaderWordConvType::COORD ){

            th[def.name]=HeaderValue( HeaderValue::float_type(th[def.name].toFloat()*coordfac) );
        }
    }

    // get elevation scaler if needed
    if( m_info.isFixedScalel() ){
        elevfac=m_info.scalel();   // this is a factor already
    }else{
        try{
        auto it=th.find("scalel");
        if( it!=th.end() ){
            auto h = th["scalel"].intValue();
            if( h<0 ){
                elevfac = -1./h;
            }
            else if( h>0 ){
                elevfac = h;
            }
        }
        }catch(...){elevfac=1.;}
    }

    // apply elevation scaler where needed
    for( auto def : defs ){

        if( def.ctype==SEGYHeaderWordConvType::ELEV){
            th[def.name]=HeaderValue( HeaderValue::float_type(th[def.name].toFloat()*elevfac) );
        }
    }

    return th;
}


SEGYReader::SEGYReader( const std::string& name,
          const SEGYInfo& info,
          size_t raw_binary_header_size,
          size_t raw_trace_header_size,
          size_t max_samples_per_trace
          ):
        m_ifile(fopen(name.c_str(), "rb")),
        m_info(info),
        m_raw_binary_header_buf(raw_binary_header_size),
        m_raw_trace_header_buf(raw_trace_header_size),
        m_raw_samples_buf( max_samples_per_trace * sizeof(sample_t)),
        m_postReadFunc(postReadFunc)
{

    if( !m_ifile) throw( SEGYFormatError("Open file failed!"));
    estimate_filesize();
    read_leading_headers();
    m_trace_count=( m_filesize - m_first_trace_pos ) / bytes_per_trace();
}



void SEGYReader::setInfo( const SEGYInfo& info ){
    m_info=info;
}


void SEGYReader::convert_binary_header(){

    m_binary_header=convert_raw_header(m_raw_binary_header_buf, m_info.binaryHeaderDef());

    if( !m_info.isOverrideSampleFormat() ){
        m_info.setSampleFormat(toSampleFormat(m_binary_header["format"].intValue()));
    }

    m_bytes_per_sample=bytesPerSample( m_info.sampleFormat());

    m_dt=0.000001 * m_binary_header["dt"].uintValue();
    m_nt=m_binary_header["ns"].uintValue();
    m_fixed_sampling=m_binary_header["fixlen"].intValue()!=0;
    int next=m_binary_header["next"].intValue();
    if(!m_info.isOverrideNEXT()){
        m_info.setNEXT(next);
    }
}

double scalerToFactor( std::int16_t scaler){

    double factor;

    if( scaler < 0 ){
       factor=1./static_cast<double>(-scaler);
    }
    else if( scaler > 0 ){
        factor=static_cast<double>(scaler);
    }
    else{           // scaler no set interpreted as no scaling which is multiply by one
        factor=1.;
    }

    return factor;
}

void SEGYReader::convert_trace_header(){
/*
    if( !m_info.isFixedScalel() ){
        std::int16_t iscalel;
        get_from_raw( &iscalel, &m_raw_trace_header_buf[0] + m_info.scalelDef().pos - 1, m_info.isSwap() ); // pos is 1-based
        m_info.setScalel( scalerToFactor(iscalel) );
    }

    if( !m_info.isFixedScalco() ){
        std::int16_t iscalco;
        get_from_raw( &iscalco, &m_raw_trace_header_buf[0] + m_info.scalcoDef().pos - 1, m_info.isSwap() ); // pos is 1-based
        m_info.setScalco( scalerToFactor(iscalco) );
    }
*/
    m_trace_header=convert_raw_header(m_raw_trace_header_buf, m_info.traceHeaderDef());

   if( !m_fixed_sampling ){
        m_dt=0.000001 * m_trace_header["dt"].uintValue();
        m_nt=m_trace_header["ns"].uintValue();
   }

   m_ft=0.001*m_trace_header["delrt"].intValue();                 // use recording delay as start time XXX
}

void SEGYReader::estimate_filesize(){
    fseek(m_ifile, 0L, SEEK_END);
    m_filesize=ftell(m_ifile);
    rewind(m_ifile);
}

void SEGYReader::read_leading_headers(){

    // read mandatory first text/ebcdic header
    SEGYTextHeader text_hdr;

    if(fread(&text_hdr[0], text_hdr.size(), 1, m_ifile)!=1){
        throw(SEGYFormatError("Reading mandatory ebcdic header failed!"));
    }
    m_postReadFunc( &text_hdr[0], text_hdr.size() );
    m_text_headers.push_back(text_hdr);

    // read binary header
    if(fread(&m_raw_binary_header_buf[0], m_raw_binary_header_buf.size(), 1, m_ifile)!=1){
        throw(SEGYFormatError("Reading binary header failed!"));
    }
    m_postReadFunc( &m_raw_binary_header_buf[0], m_raw_binary_header_buf.size() );
    convert_binary_header();

    // read optional text headers as specified in binary header
    if( m_info.next()<0 ) throw SEGYFormatError("variable number of extended textual headers is not implemented yet!");
    for( int i=0; i<m_info.next(); i++){
        if( fread(&text_hdr[0], text_hdr.size(), 1, m_ifile)!=1){
            throw(SEGYFormatError("Reading extended ebcdic header failed!"));
        }
        m_postReadFunc(&text_hdr[0], text_hdr.size());
        m_text_headers.push_back(text_hdr);
    }

    // store beginning of traces
    m_first_trace_pos=m_current_position=ftell(m_ifile);
    m_cur_trace=0;
    m_is_next_trace_header=true;
}


void SEGYReader::seek_trace(const ssize_t& n){

    if( n>=m_trace_count ){
        throw SEGYFormatError("Trying to access trace after eof!");
    }
    ssize_t pos=m_first_trace_pos + n * bytes_per_trace() ;
    fseek( m_ifile, pos, SEEK_SET);
    m_current_position=ftell(m_ifile);
    m_cur_trace=n;
    m_is_next_trace_header=true;
}


Header SEGYReader::read_trace_header(){

    if(!m_is_next_trace_header){
        throw( SEGYFormatError("Attempting to read trace header from illegal position!"));
    }


    if(fread(&m_raw_trace_header_buf[0], m_raw_trace_header_buf.size(), 1, m_ifile)!=1){
        throw(SEGYFormatError("Reading trace header failed!"));
    }
    m_postReadFunc(&m_raw_trace_header_buf[0], m_raw_trace_header_buf.size());
    convert_trace_header();

    m_current_position=ftell(m_ifile);
    m_is_next_trace_header=false;

    return m_trace_header;
}


void SEGYReader::read_samples(){

    if(m_is_next_trace_header){
        throw( SEGYFormatError("Attempting to read samples from illegal position!"));
    }

    ssize_t nbytes=m_bytes_per_sample * m_nt;
    if( fread( &m_raw_samples_buf[0], nbytes, 1, m_ifile )!=1){
        throw SEGYFormatError("Reading samples failed!");
    }
    //don't decrypt samples as they are not encrypted, only  headers are
    //m_postReadFunc(&m_raw_samples_buf[0], nbytes);
    m_current_position=ftell(m_ifile);
}

void SEGYReader::convert_raw_samples( Trace& trc){
    switch( m_info.sampleFormat() ){
    case SEGYSampleFormat::IBM: convert_ibm_to_samples( &m_raw_samples_buf[0], trc.samples(), m_nt, m_info.isSwap() );break;
    case SEGYSampleFormat::IEEE: convert_ieee_to_samples( &m_raw_samples_buf[0], trc.samples(), m_nt, m_info.isSwap() );break;
    case SEGYSampleFormat::INT4: convert_int4_to_samples( &m_raw_samples_buf[0], trc.samples(), m_nt, m_info.isSwap());break;
    case SEGYSampleFormat::INT2: convert_int2_to_samples( &m_raw_samples_buf[0], trc.samples(), m_nt, m_info.isSwap());break;
    case SEGYSampleFormat::INT1: convert_int1_to_samples( &m_raw_samples_buf[0], trc.samples(), m_nt, m_info.isSwap());break;
    case SEGYSampleFormat::FIXP: convert_fixp_to_samples( &m_raw_samples_buf[0], trc.samples(), m_nt, m_info.isSwap());break;
    default: throw SEGYFormatError("Unsupported sample format code!");break;
    }
}

Trace SEGYReader::read_trace(){

    if(m_is_next_trace_header ){
        read_trace_header();
    }

    Trace trc(m_ft, m_dt, m_nt, m_trace_header );      // sampling params m_dt and m_nt are updated in convert_trace_header

    read_samples();

    convert_raw_samples(trc);

    m_is_next_trace_header=true;

    m_cur_trace++;

    return trc;
}


std::shared_ptr<Gather> SEGYReader::read_gather( const std::string& key, const size_t max_traces){

    std::shared_ptr<Gather> gather( new Gather());

    if( m_cur_trace>=m_trace_count) return gather;      // reached the end, return empty gather

    gather->push_back( read_trace() );

    Header firstHeader=gather->front().header();
    Header::iterator it=firstHeader.find(key);
    if( it==firstHeader.end()) throw SEGYFormatError("Gather key does not exist!!!");
    HeaderValue keyValue=it->second;

    while( m_cur_trace<m_trace_count && gather->size()<max_traces){

        read_trace_header();

        if( m_trace_header[key] != keyValue) break;
        gather->push_back( read_trace() );
    }


    return gather;
}

void convert_ibm_to_samples( char* rawbuf, Trace::Samples& dest, size_t n, bool chend){

    int32_t fconv, fmant, t;

    int32_t* src=(int32_t*)rawbuf;

    for ( size_t i=0; i<n; i++)
    {

            fconv = *src;

            if (chend)      // XXX
                    fconv = (fconv<<24) | ((fconv>>24)&0xff) | ((fconv&0xff00)<<8) | ((fconv&0xff0000)>>8);

            if (fconv)
            {
                fmant = 0x00ffffff & fconv;

                if( fmant==0){
                    fconv=0;
                }
                else{
                    t = (int32_t) ((0x7f000000 & fconv) >> 22) - 130;

                    while (!(fmant & 0x00800000)) { --t; fmant <<= 1; }


                    if (t > 254)
                            fconv = (0x80000000 & fconv) | 0x7f7fffff;
                    else if (t <= 0)
                            fconv = 0;
                    else
                            fconv = (0x80000000 & fconv) |(t << 23)|(0x007fffff & fmant);
                }
            }

            dest[i] = *((float*)&(fconv));

            src++;
    }

}


void convert_ieee_to_samples( char* rawbuf, Trace::Samples& samples, size_t n, bool swap ){

    static_assert( sizeof(Trace::sample_type)==sizeof(int32_t), "Assuming float has 4 bytes!");
    char* src=rawbuf;
    for( size_t i=0; i<n; i++){

        get_from_raw( (int32_t*)(&samples[i]), src, swap);
        src+=4;
    }
}

void convert_int4_to_samples( char* rawbuf, Trace::Samples& samples, size_t n, bool chend){

    char* src=rawbuf;
    for( size_t i=0; i<n; i++){

        int32_t i32;
        get_from_raw( &i32, src, chend);
        samples[i]=static_cast<Trace::sample_type>(i32);
        src+=4;
    }
}

void convert_int2_to_samples( char* rawbuf, Trace::Samples& samples, size_t n, bool chend){

    char* src=rawbuf;
    for( size_t i=0; i<n; i++){

        int16_t i16;
        get_from_raw( &i16, src, chend);
        samples[i]=static_cast<Trace::sample_type>(i16);
        src+=2;
    }
}

void convert_int1_to_samples( char* rawbuf, Trace::Samples& samples, size_t n, bool chend){

    char* src=rawbuf;
    for( size_t i=0; i<n; i++){

        int8_t i8=*src;
        samples[i]=static_cast<Trace::sample_type>(i8);
        src++;
    }
}

void convert_fixp_to_samples( char* rawbuf, Trace::Samples& dest, size_t n, bool chend){
    throw SEGYFormatError("NOT IMPLEMENTED: convert_fixp_to_samples!");
}

}
