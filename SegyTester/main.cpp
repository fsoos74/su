#include <QCoreApplication>


#include<segyreader.h>
#include<segywriter.h>
#include<segyinfo.h>
#include<xsireader.h>
#include<segy_traceheader_def.h>
#include<segy_binary_header_def.h>
#include<segysampleformat.h>
#include<trace.h>
#include<header.h>
#include<seis_bits.h>

#include<QFile>


using namespace seismic;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
/*
    const char* INPUT_FILE="/DATA/avo_data/GOM\ Data/GOM-3D-cdp_pstm_avo.sgy";
    const char* INFO_FILE="/home/fsoos/GOM_AVO/seismic/gather.xsi";
    const char* OUTPUT_FILE="/home/fsoos/disguised_gathers.sgy";
    bool PRESTACK=true;
*/
    const char* INPUT_FILE="/DATA/avo_data/GOM\ Data/coastland_operations_llc-texlamerge2010-12-0103-3D-stack_pstm_enhanced-001.sgy";
    const char* INFO_FILE="/home/fsoos/GOM_AVO/seismic/stack-pstm-enhanced.xsi";
    const char* OUTPUT_FILE="/home/fsoos/disguised_stack.sgy";
    bool PRESTACK=false;

    double REFX=3420000;
    double REFY=570000;
    double SCALE=10;


    SEGYInfo iinfo;
    XSIReader xsireader(iinfo);
    QFile xsifile(INFO_FILE);
    xsifile.open(QFile::ReadOnly | QFile::Text);
    if( !xsireader.read(&xsifile) ){
        std::cerr<<"Error reading xsi file "<<INFO_FILE<<std::endl;
        return 1;
    }

    SEGYReader reader(INPUT_FILE, iinfo);

    SEGYInfo oinfo; // default segy
    oinfo.setSampleFormat(SEGYSampleFormat::IEEE);  // NEED to update binary header accordingly
    seismic::SEGYTextHeaderStr textHeaderStr;
    textHeaderStr.push_back("Trace Header Definition:");
    textHeaderStr.push_back("Inline                         bytes 189-192");
    textHeaderStr.push_back("Crossline          b           bytes 193-196");
    textHeaderStr.push_back("CDP X-Coordinate               bytes 181-184");
    textHeaderStr.push_back("CDP Y-Coordinate               bytes 185-188");
    seismic::SEGYTextHeader textHeader=seismic::convertToRaw(textHeaderStr);

    Header bh=reader.binaryHeader();
    bh["format"]=HeaderValue(HeaderValue::int_type(5)); //SEGYSampleFormat::IEEE; NEED function to convert between SampleFormat and int

    SEGYWriter writer(OUTPUT_FILE, oinfo, textHeader, bh);


    size_t N=reader.trace_count();
    while( reader.current_trace() < N){
        Trace trace=reader.read_trace();

        size_t ns=trace.header()["ns"].uintValue();
        trace.header()["ns"]=HeaderValue(HeaderValue::uint_type(trace.size())); // force header to match trace size

        double x=trace.header()["cdpx"].floatValue();
        x-=REFX;
        trace.header()["cdpx"]=seismic::HeaderValue::makeFloatValue(x*SCALE);
        double y=trace.header()["cdpy"].floatValue();
        y-=REFY;
        trace.header()["cdpy"]=seismic::HeaderValue::makeFloatValue(y*SCALE);

        if( PRESTACK){
            x=trace.header()["sx"].floatValue();
            x-=REFX;
            trace.header()["sx"]=seismic::HeaderValue::makeFloatValue(x*SCALE);
            y=trace.header()["sy"].floatValue();
            y-=REFY;
            trace.header()["sy"]=seismic::HeaderValue::makeFloatValue(y*SCALE);


            x=trace.header()["gx"].floatValue();
            x-=REFX;
            trace.header()["gx"]=seismic::HeaderValue::makeFloatValue(x*SCALE);
            y=trace.header()["gy"].floatValue();
            y-=REFY;
            trace.header()["gy"]=seismic::HeaderValue::makeFloatValue(y*SCALE);
        }
        else{
            // for poststack data
            trace.header()["sx"]=trace.header()["cdpx"];
            trace.header()["sy"]=trace.header()["cdpy"];
            trace.header()["gx"]=trace.header()["cdpx"];
            trace.header()["gy"]=trace.header()["cdpy"];
            trace.header()["offset"]=seismic::HeaderValue::makeFloatValue(0);
        }

        writer.write_trace(trace);

        if( (reader.current_trace()%1000) ==0){
            std::cout<<"trace #"<<reader.current_trace()<<" of "<<N<<std::endl;
        }
    }

    return a.exec();
}
