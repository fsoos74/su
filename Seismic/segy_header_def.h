#ifndef SEGY_HEADER_DEF_H
#define SEGY_HEADER_DEF_H

#include<string>
#include<QString>

namespace seismic{

// type of header value
enum class SEGYHeaderWordDataType{ INT16, // 2 byte signed integer
                                INT32, // 4 byte signed integer
                                UINT16,// 2 byte unsigned integer
                                BUF6   // 6 byte block
                           };

QString toQString(SEGYHeaderWordDataType);
SEGYHeaderWordDataType toDataType(const QString&);

// describes how values should be processed/converted
enum class SEGYHeaderWordConvType{    PLAIN, // use value as is
                                    ELEV,  // elevation,  scale by scalel
                                    COORD  // coordinate, scale by scalco
                           };

QString toQString(SEGYHeaderWordConvType);
SEGYHeaderWordConvType toConvType(const QString&);


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

