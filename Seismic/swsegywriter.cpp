#include "swsegywriter.h"
#include "seis_bits.h"

#include<iostream>
#include<cstring> // strlen

namespace seismic{


SWSEGYWriter::SWSEGYWriter( const std::string& name,
                             const SEGYInfo& info,
                             const SEGYTextHeader& textHeader,   //only support 1 ebcdic/textual header block for now
                             const Header& binaryHeader,
                            const std::string& lineName)
    :SEGYWriter( name, info, textHeader, binaryHeader), m_lineName(lineName){

    std::cout<<"SWSEGYWriter::SWSEGYWriter"<<std::endl<<std::flush;
}


void SWSEGYWriter::process_raw_binary_header(std::vector<char>& buf){

    std::cout<<"SWSEGYWriter::process_raw_binary_header(std::vector<char>& buf)"<<std::endl<<std::flush;

    std::int16_t fmt=1;  // Seisware uses 1 as format code for ieee

    auto fmtdef=std::find_if( info().binaryHeaderDef().begin(),
                              info().binaryHeaderDef().end(),
                              [](SEGYHeaderWordDef def){return def.name=="format";}
                              );
    if( fmtdef==info().binaryHeaderDef().end()){
        throw SEGYFormatError("No format given in SWSEGY binary header definition!");
    }

    std::cout<<"Writing "<<fmt<<" to "<<fmtdef->pos-1<<std::endl<<std::flush;


    put_to_raw( &fmt, &buf[0] + fmtdef->pos-1, info().isSwap() );


    std::cout<<"Writing linename to raw bh"<<std::endl<<std::flush;
    //char ln[]="FluidFactor\0";

    put_to_raw(m_lineName.c_str(), m_lineName.length(), &buf[0]+301 - 1);


}

}
