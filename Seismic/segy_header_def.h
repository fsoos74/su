#ifndef SEGY_HEADER_DEF_H
#define SEGY_HEADER_DEF_H

namespace seismic{

// type of header value
enum SEGYHeaderWordDataType{ SEGY_INT16, // 2 byte signed integer
                             SEGY_INT32, // 4 byte signed integer
                             SEGY_UINT16,// 2 byte unsigned integer
                             SEGY_BUF6   // 6 byte block
                           };

// describes how values should be processed/converted
enum SEGYHeaderWordConvType{    SEGY_PLAIN, // use value as is
                                SEGY_ELEV,  // elevation,  scale by scalel
                                SEGY_COORD  // coordinate, scale by scalco
                           };


// description of a segy header value
struct SEGYHeaderWordDef{
    std::string             name;
    SEGYHeaderWordConvType	ctype;
    SEGYHeaderWordDataType	dtype;
    size_t                  pos;
    std::string             descr;

    SEGYHeaderWordDef( std::string              iname,
                       SEGYHeaderWordConvType   ictype,
                       SEGYHeaderWordDataType	idtype,
                       size_t                   ipos,
                       std::string              idescr):
                            name(iname), ctype(ictype), dtype(idtype), pos(ipos), descr(idescr){}

};


}


#endif // SEGY_HEADER_DEF_H

