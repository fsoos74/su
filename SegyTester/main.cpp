#include <QCoreApplication>



#include<segyreader.h>
#include<segywriter.h>
#include<segyinfo.h>
#include<segy_traceheader_def.h>
#include<segy_binary_header_def.h>
#include<segysampleformat.h>
#include<trace.h>
#include<header.h>
#include<seis_bits.h>


using namespace seismic;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    const char* INAME="/DATA/avo_data/brooks/BRfnl.sgy";
    SEGYInfo iinfo;
    SEGYReader reader(INAME, iinfo);

    const char* ONAME="/home/fsoos/wtest.sgy";
    SEGYInfo oinfo=iinfo;
    oinfo.setSampleFormat(SEGYSampleFormat::IEEE);  // NEED to update binary header accordingly
    SEGYTextHeader ebcdic=reader.textHeaders()[0];

    Header bh=reader.binaryHeader();
    bh["format"]=HeaderValue(HeaderValue::int_type(5)); //SEGYSampleFormat::IEEE; NEED function to convert between SampleFormat and int

    SEGYWriter writer(ONAME, oinfo, ebcdic, bh);


    float min=std::numeric_limits<float>::max();
    float max=std::numeric_limits<float>::lowest();

    size_t N=reader.trace_count();
    while( reader.current_trace() < N){
        Trace trace=reader.read_trace();

        size_t ns=trace.header()["ns"].uintValue();
        trace.header()["ns"]=HeaderValue(HeaderValue::uint_type(trace.size())); // force header to match trace size

        std::cout<<"cdp="<<trace.header()["cdp"].intValue()<<std::endl;
        writer.write_trace(trace);

        for( auto x : trace.samples()){
            if( x<min ) min=x;
            if( x>max ) max=x;
        }
        std::cout<<"trace #"<<reader.current_trace()<<" of "<<N<<std::endl;
    }

    std::cout<<"Amplitude Range: "<<min<<" - "<<max<<std::endl;

    return a.exec();
}
