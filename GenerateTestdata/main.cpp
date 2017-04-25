#include <QCoreApplication>

#include<trace.h>
#include<segywriter.h>
#include<segy_binary_header_def.h>
#include<segy_text_header.h>
#include<segy_traceheader_def.h>

void createSEGY(){

    auto filename="/home/fsoos/5-10-20-40-80hz.sgy";

    int ilmin=1;
    int ilmax=100;
    int xlmin=1001;
    int xlmax=1100;
    int t0ms=0;
    int dtms=4;
    int ns=1001;

    // coordinate system definition
    int il0=1;          // inline at origin
    int xl0=1001;       // crossline at origin
    double x0=1000000;  // x-coord of origin
    double y0=2000000;  // y-coord of origin
    double il_dx=100;   // x-increment per inline
    double il_dy=0;     // y-increment per inline
    double xl_dx=0;     // x-increment per crossline
    double xl_dy=100;   // y-increment per crossline
    const int SCALCO=10;

    seismic::SEGYTextHeaderStr textHeaderStr;
    textHeaderStr.push_back("SEGY testdata for AVO-Detect");
    textHeaderStr.push_back(QString("Inlines:     %1 - %2").arg(ilmin).arg(ilmax).toStdString());
    textHeaderStr.push_back(QString("Crosslines:  %1 - %2").arg(xlmin).arg(xlmax).toStdString());
    textHeaderStr.push_back(QString("Time [ms]:   %1 - %2").arg(t0ms).arg(t0ms+(ns-1)*dtms).toStdString());
    textHeaderStr.push_back("");
    textHeaderStr.push_back("Trace Header Definition:");
    textHeaderStr.push_back("Time of first sample [ms]  bytes 109-110");
    textHeaderStr.push_back("Inline                     bytes 189-192");
    textHeaderStr.push_back("Crossline                  bytes 193-196");
    textHeaderStr.push_back("X-Coordinate               bytes 181-184");
    textHeaderStr.push_back("Y-Coordinate               bytes 185-188");
    auto ebcdic=seismic::convertToRaw(textHeaderStr);

    seismic::Header bhdr;
    bhdr["format"]=seismic::HeaderValue::makeIntValue( toInt( seismic::SEGYSampleFormat::IEEE) );
    bhdr["ns"]=seismic::HeaderValue::makeUIntValue(ns);
    bhdr["dt"]=seismic::HeaderValue::makeUIntValue(dtms*1000);  // microseconds

    seismic::SEGYInfo info;
    info.setScalco(-1./SCALCO);
    info.setSwap(true);     // XXX for little endian machines , NEED to automatically use endianess of machine a compile time
    info.setSampleFormat(seismic::SEGYSampleFormat::IEEE);
    seismic::SEGYWriter writer( filename, info, ebcdic, bhdr );
    writer.write_leading_headers();

    seismic::Trace trace(0.001*t0ms, 0.001*dtms, ns );  // trace times are in seconds
    seismic::Header& thdr=trace.header();
    seismic::Trace::Samples& samples=trace.samples();

    thdr["delrt"]=seismic::HeaderValue::makeIntValue(t0ms); // start time
    thdr["ns"]=seismic::HeaderValue::makeUIntValue(ns);
    thdr["dt"]=seismic::HeaderValue::makeUIntValue(1000*dtms);
    thdr["scalco"]=seismic::HeaderValue::makeIntValue(SCALCO);

    int n=0;
    std::vector<double> freqs={ 5, 10, 20, 40, 80 };
    for( int iline=ilmin; iline<=ilmax; iline++){

        for( int xline=xlmin; xline<=xlmax; xline++){

            n++;

            double x= x0 + (iline-il0)*il_dx + (xline-xl0)*xl_dx;
            double y= y0 + (xline-xl0)*il_dy + (xline-xl0)*xl_dy;
            int cdp=(iline-ilmin)*(xlmax-xlmin) + xline-xlmin + 1;

            thdr["cdp"]=seismic::HeaderValue::makeIntValue(cdp);
            thdr["tracr"]=seismic::HeaderValue::makeIntValue(n);
            thdr["tracl"]=seismic::HeaderValue::makeIntValue(n);
            thdr["iline"]=seismic::HeaderValue::makeIntValue(iline);
            thdr["xline"]=seismic::HeaderValue::makeIntValue(xline);
            thdr["sx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["sy"]=seismic::HeaderValue::makeFloatValue(y);
            thdr["gx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["gy"]=seismic::HeaderValue::makeFloatValue(y);
            thdr["cdpx"]=seismic::HeaderValue::makeFloatValue(x);
            thdr["cdpy"]=seismic::HeaderValue::makeFloatValue(y);

            for( int i=0; i<ns; i++){

               double t = 0.001 * (t0ms + i*dtms + (xline-xlmin) );

               double s=0;
               for( auto f:freqs){
                    s+=sin( 2*M_PI*f*t );
               }
               samples[i]=s;
            }

            writer.write_trace(trace);
        }

    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    createSEGY();

    return a.exec();
}
