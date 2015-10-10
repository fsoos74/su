#include<segy_text_header.h>

#include<ebcdic.h>

namespace seismic{

SEGYTextHeaderStr convertToString( const SEGYTextHeader& thdr, bool conv_ebcdic, const size_t nrows, const size_t ncols){

    if( nrows*ncols>thdr.size()) throw std::runtime_error("Access to text header with too large number of rows and cols!");

    SEGYTextHeaderStr res;
    res.reserve( nrows);

    std::vector<char> line(ncols);
    for( size_t row=0; row<nrows; row++ ){
        // copying would not be necessary for ascii headers but they are rare so better avoid a special case
        for( size_t col=0; col<ncols; col++)
            line[col]=thdr[row*ncols+col];
        if(conv_ebcdic){
            for( size_t col=0; col<ncols; col++ ){
                unsigned char e=static_cast<unsigned char>(line[col]); // ebcdic has 256 chars
                line[col]=EBCDIC_ASCII_TABLE[e];
            }
        }

        res.push_back(std::string(&line[0], ncols));

    }


    return res;
}

}

