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

SEGYTextHeader convertToRaw( const SEGYTextHeaderStr& str, bool conv_ebcdic, const size_t nrows, const size_t ncols){

    SEGYTextHeader hdr;
    if( nrows*ncols>hdr.size()) throw std::runtime_error("Access to text header with too large number of rows and cols!");

    unsigned char fill=(conv_ebcdic)? ASCII_EBCDIC_TABLE[' '] : ' ';
    for( auto& c : hdr ) c=fill;

    for( size_t row=0; row<str.size(); row++){

        const std::string& s=str[row];
        size_t n=s.length();
        if( n>ncols) n=ncols;               // silently fix too long lines

        for( size_t col=0; col<n; col++){

            unsigned char c=static_cast<unsigned char>(s[col]); // ebcdic has 256 chars
            if( conv_ebcdic){
                c=ASCII_EBCDIC_TABLE[c];
            }

            hdr[row*ncols + col]=c;

        }
    }

    return hdr;

}

}

