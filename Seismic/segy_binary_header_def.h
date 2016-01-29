#ifndef SEGY_BINARY_HEADER_DEF_H
#define SEGY_BINARY_HEADER_DEF_H

#include <vector>

#include "segy_header_def.h"

namespace seismic{

static const std::vector<SEGYHeaderWordDef> SEGY_DEFAULT_BINARY_HEADER_DEF{

    {"jobid", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,   1, R"(Job identification number.
    )"},
    {"lineno", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,   5, R"(Line number.  For 3-D poststack data, this will typically contain the in-line number.
    )"},
    {"realno", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT32,   9, R"(Reel number.
    )"},
    {"nens", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  13, R"(Number of data traces per ensemble.  Mandatory for prestack data.
    )"},
    {"naux", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  15, R"(Number of auxiliary traces per ensemble.  Mandatory for prestack data.
    )"},
    {"dt", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  17, R"(Sample interval in microseconds (μs).  Mandatory for all data types.
    )"},
    {"odt", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  19, R"(Sample interval in microseconds  (μs) of original field recording.
    )"},
    {"ns", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  21, R"(Number of samples per data trace.  Mandatory for all types of data.
        Note: The sample interval and number of samples in the Binary File Header
        should be for the primary set of seismic data traces in the file.
    )"},
    {"ons", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  23, R"(Number of samples per data trace for original field recording.
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
    {"fold", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16,  27, R"(Ensemble fold — The expected number of data traces per trace ensemble
        (e.g. the CMP fold).  Highly recommended for all types of data.
    )"},
    {"scode", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  29, R"(Trace sorting code (i.e. type of ensemble) :
        -1 = Other (should be explained in user Extended Textual File Header stanza
         0 = Unknown
         1 = As recorded (no sorting)
         2 = CDP ensemble
         3 = Single fold continuous profile
         4 = Horizontally stacked
         5 = Common source point
         6 = Common receiver point
         7 = Common offset point
         8 = Common mid-point
         9 = Common conversion point
        Highly recommended for all types of data.
    )"},
    {"vsum", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  31, R"(Vertical sum code:
        1 = no sum,
        2 = two sum,
        ...,
        N = M-1 sum  (M = 2 to 32,767)
    )"},
    {"sstart", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  33, R"(Sweep frequency at start (Hz).
    )"},
    {"send", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  35, R"(Sweep frequency at end (Hz).
    )"},
    {"slen", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  37, R"(Sweep length (ms).
    )"},
    {"scode", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  39, R"(Sweep type code:
        1 = linear
        2 = parabolic
        3 = exponential
        4 = other
    )"},
    {"strc", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  41, R"(Trace number of sweep channel.
    )"},
    {"stslen", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  43, R"(Sweep trace taper length in milliseconds at start if tapered 	(the taper starts at zero time and is effective for this length).
    )"},
    {"stelen", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  45, R"(Sweep trace taper length in milliseconds at end (the ending taper starts at
        sweep length minus the taper length at end).
    )"},
    {"sttyp", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  47, R"(Taper type:
        1 = linear
        2 = cos
        3 = other
    )"},
    {"corr", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  49, R"(Correlated data traces:
        1 = no
        2 = yes
    )"},
    {"gainr", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  51, R"(Binary gain recovered:
        1 = yes
        2 = no
    )"},
    {"ampr", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  53, R"(Amplitude recovery method:
        1 = none
        2 = spherical divergence
        3 = AGC
        4 = other
    )"},
    {"msys", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  55, R"(Measurement system: Highly recommended for all types of data.
      If Location
        Data stanzas are included in the file, this entry must agree with the Location
        Data stanza.  If there is a disagreement, the last Location Data stanza is the
        controlling authority.
        1 = Meters
        2 = Feet
    )"},
    {"polar", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  57, R"(Impulse signal polarity
        1 = Increase in pressure or upward geophone case movement gives negative
        number on tape.
        2 = Increase in pressure or upward geophone case movement gives positive
        number on tape.
    )"},
    {"vpcode", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16,  59, R"(Vibratory polarity code:
        Seismic signal lags pilot signal by:
        1 = 337.5° to   22.5°
        2 =   22.5° to   67.5°
        3 =   67.5° to 112.5°
        4 = 112.5° to 157.5°
        5 = 157.5° to 202.5°
        6 = 202.5° to 247.5°
        7 = 247.5° to 292.5°
        8 = 292.5° to 337.5°
    )"},
    {"revision", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::UINT16, 301, R"(SEG Y Format Revision Number.  This is a 16-bit unsigned value with a
        Q-point between the first and second bytes.  Thus for SEG Y Revision 1.0, as
        defined in this document, this will be recorded as 0100
        .  This field is
        mandatory for all versions of SEG Y, although a value of zero indicates
        “traditional” SEG Y conforming to the 1975 standard.
    )"},
    {"fixlen", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16, 303, R"(Fixed length trace flag.  A value of one indicates that all traces in this SEG Y
        file are guaranteed to have the same sample interval and number of samples,
        as specified in Textual File Header bytes 3217-3218 and 3221-3222.  A value
        of zero indicates that the length of the traces in the file may vary and the
        number of samples in bytes 115-116 of the Trace Header must be examined to
        determine the actual length of each trace. This field is mandatory for all
        versions of SEG Y, although a value of zero indicates “traditional” SEG Y
        conforming to the 1975 standard.
    )"},
    {"next", SEGYHeaderWordConvType::PLAIN, SEGYHeaderWordDataType::INT16, 305, R"(Number of 3200-byte, Extended Textual File Header records following the
        Binary Header.  A value of zero indicates there are no Extended Textual File
        Header records)"}

};

}

#endif // SEGY_BINARY_HEADER_DEF_H

