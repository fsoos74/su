#ifndef SEISWARE_HEADER_DEF_H
#define SEISWARE_HEADER_DEF_H

#include <vector>

#include "segy_header_def.h"

namespace seismic{

static const std::vector<SEGYHeaderWordDef> SEISWARE_BINARY_HEADER_DEF{

    {"dt", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  17, R"(Sample interval in microseconds (μs).  Mandatory for all data types.
    )"},
    {"ns", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  21, R"(Number of samples per data trace. Seisware requires this to be constant for all traces.
    )"},
    {"format", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  25, R"(Data sample format code.  Mandatory for all data.
        1 = 4-byte IBM floating-point
        2 = 4-byte, two's complement integer
        3 = 2-byte, two's complement integer
        4 = 4-byte fixed-point with gain (obsolete)
        5 = 4-byte IEEE floating-point
        6 = Not currently used
        7 = Not currently used
        8 = 1-byte, two's complement integer
    )"},
    {"sswnd", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  101,
                R"(Start of statistics window in microseconds.
    )"},
    {"eswnd", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  105,
                R"(End of statistics window in microseconds.
    )"},
    {"speak", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  109,
                R"(Peak amplitude in statistics window.
    )"},
    {"savg", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  113,
                R"(Average absolute amplitude in statistics window.
    )"},
    {"srms", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  117,
                R"(RMS amplitude in statistics window.
    )"},
    {"startx", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  201,
                R"(Starting x, i.e. trace (xline).
    )"},
    {"starty", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  205,
                R"(Starting y, i.e. line (inline). This must be zero for 2D data.
    )"},
    {"startz", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  209,
                R"(Starting z, i.e. time in microseconds.
    )"},
    {"endx", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  213,
                R"(ending x, i.e. trace (xline).
    )"},
    {"endy", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  217,
                R"(ending y, i.e. line (inline). This must be zero for 2D data.
    )"},
    {"endz", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  221,
                R"(ending z, i.e. time in microseconds.
    )"},

    // added the following keys without knowing whatthey are as test
    {"dum1", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  225,
                R"(DUMMY.
    )"},
    {"dum2", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  229,
                R"(DUMMY.
    )"},
    {"dum3", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  233,
                R"(DUMMY.
    )"},

    {"dum5", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  161,
                R"(DUMMY.
    )"},

    {"totphase", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  341,
                R"(The total phase rotation in degrees that has been applied to the entire line. The default value should be zero.
    )"},
    {"totgain", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  345,
                R"(The total gain applied to the entire line. The default value should be one.
    )"},
    {"gainexp", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  349,
                R"(The exponent of the Total Gain applied to the line. The default value should be one.
    )"},

    // added the following key without knowing what it is as test
    {"dum4", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  353,
                R"(DUMMY.
    )"},


    {"corner1", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  357,
                R"(Trace offset (starting from zero) of the first corner of a 3D.
                 The first corner is the lowest line/trace.
                 This is used for geometry calculations.
                 This value should be zero for 2D data.
    )"},
    {"corner2", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  361,
                R"(Trace offset (starting from zero) of the second corner of a 3D.
                 The second corner is the lowest line and highest trace. "
                 This is used for geometry calculations.
                 This value should be zero for 2D data.
    )"},
    {"corner3", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  365,
                R"(Trace offset (starting from zero) of the third corner of a 3D.
                 The third corner is the highest line and highest trace.
                 This is used for geometry calculations.
                 This value should be zero for 2D data.
    )"},
    {"geometry", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  393,
                R"(Geometry of the data in the file. This will be 2 for 2D data and 3 for 3D data.
    )"},
    {"swflag", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT8,  399,
                R"(Flag indicating whether this is a SeisWare format SEG-Y file. This byte must contain the value 91.
    )"}
};




static const std::vector<SEGYHeaderWordDef> SEISWARE_TRACE_HEADER_DEF{
    {"ns", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16, 115, R"( Number of samples in this trace
    )"},
    {"dt", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16, 117, R"( Sample interval of this trace in microseconds
    )"},
    {"tracl", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,   1, R"( Trace sequence number within line
    )"},
    {"iline", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  9,
                R"(Line number of the trace. Only used for 3D data.
    )"},
    {"xline", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  13,
                R"(Trace number of the trace. Only used for 3D data.
    )"},
    {"shotseq", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE, 17,
                R"(Shot sequence number of the trace. Only used for 2D data.
    )"},
    {"cdp", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,  21,
                R"(CDP number of the trace. Only used for 2D data.
    )"},
    {"x", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE, 81,
                R"(X coordinate of the trace.
    )"},
    {"y", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE, 85,
                R"(Y coordinate of the trace.
    )"},
    {"speak", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  161,
                R"(Peak amplitude in statistics window.
    )"},
    {"savg", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  165,
                R"(Average absolute amplitude in statistics window.
    )"},
    {"srms", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::IEEE,  169,
                R"(RMS amplitude in statistics window.
    )"},
};






}

#endif // SEISWARE_HEADER_DEF_H
