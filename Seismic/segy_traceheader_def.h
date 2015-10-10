#ifndef SEGY_TRACEHEADER_DEF_H
#define SEGY_TRACEHEADER_DEF_H


#include <vector>

#include "segy_header_def.h"

namespace seismic{

static const std::vector<SEGYHeaderWordDef> SEGY_DEFAULT_TRACE_HEADER_DEF{

    {"tracl", SEGY_PLAIN, SEGY_INT32,   1, R"( Trace sequence number within line
    )"},
    {"tracr", SEGY_PLAIN, SEGY_INT32,   5, R"( Trace sequence number within reel
    )"},
    {"fldr", SEGY_PLAIN, SEGY_INT32,   9, R"( Original field record number
    )"},
    {"tracf", SEGY_PLAIN, SEGY_INT32,  13, R"( Trace sequence number within original field record
    )"},
    {"ep", SEGY_PLAIN, SEGY_INT32,  17, R"( Energy source point number
    )"},
    {"cdp", SEGY_PLAIN, SEGY_INT32,  21, R"( CDP ensemble number
    )"},
    {"cdpt", SEGY_PLAIN, SEGY_INT32,  25, R"( Trace sequence number within CDP ensemble
    )"},
    {"trid", SEGY_PLAIN, SEGY_INT16,  29, R"( Trace identification code:
        -1 = Other
        0 = Unknown
        1 = seismic data
        2 = dead
        3 = dummy
        4 = time break
        5 = uphole
        6 = sweep
        7 = timing
        8 = water break
        9 = Near-field gun signature
        10 = Far-field gun signature
        11 = Seismic pressure sensor
        12 = Multicomponent seismic sensor - Vertical component
        13 = Multicomponent seismic sensor - Cross-line component
        14 = Multicomponent seismic sensor - In-line component
        15 = Rotated multicomponent seismic sensor - Vertical component
        16 = Rotated multicomponent seismic sensor - Transverse component
        17 = Rotated multicomponent seismic sensor - Radial component
        18 = Vibrator reaction mass
        19 = Vibrator baseplate
        20 = Vibrator estimated ground force
        21 = Vibrator reference
        22 = Time-velocity pairs
    )"},
    {"nvs", SEGY_PLAIN, SEGY_INT16,  31, R"( Number of vertically summed traces yielding this trace
    )"},
    {"nhs", SEGY_PLAIN, SEGY_INT16,  33, R"( Number of horizontally stacked traces yielding this trace
    )"},
    {"duse", SEGY_PLAIN, SEGY_INT16,  35, R"( Data use (1 = production, 2 = test)
    )"},
    {"offset", SEGY_COORD, SEGY_INT32,  37, R"( Distance from source point to receiver group
    )"},
    {"gelev", SEGY_ELEV, SEGY_INT32,  41, R"( Receiver group elevation
    )"},
    {"selev", SEGY_ELEV, SEGY_INT32,  45, R"( Surface elevation at source
    )"},
    {"sdepth", SEGY_ELEV, SEGY_INT32,  49, R"( Source depth below surface
    )"},
    {"gdel", SEGY_ELEV, SEGY_INT32,  53, R"( Datum elevation at receiver group
    )"},
    {"sdel", SEGY_ELEV, SEGY_INT32,  57, R"( Datum elevation at source
    )"},
    {"swdep", SEGY_ELEV, SEGY_INT32,  61, R"( Water depth at source
    )"},
    {"gwdep", SEGY_ELEV, SEGY_INT32,  65, R"( Water depth at receiver group
    )"},
    {"scalel", SEGY_PLAIN, SEGY_INT16,  69, R"( Scalar for elevations and depths (+ = multiplier, - = divisor)
    )"},
    {"scalco", SEGY_PLAIN, SEGY_INT16,  71, R"( Scalar for coordinates (+ = multiplier, - = divisor)
    )"},
    {"sx", SEGY_COORD, SEGY_INT32,  73, R"( X source coordinate
    )"},
    {"sy", SEGY_COORD, SEGY_INT32,  77, R"( Y source coordinate
    )"},
    {"gx", SEGY_COORD, SEGY_INT32,  81, R"( X receiver group coordinate
    )"},
    {"gy", SEGY_COORD, SEGY_INT32,  85, R"( Y receiver group coordinate
    )"},
    {"counit", SEGY_PLAIN, SEGY_INT16,  89, R"( Coordinate units
        1 = Length in meters or feet
        2 = arc seconds
        3 = decimal degrees
        4 = degrees, minutes, seconds
    )"},
    {"wevel", SEGY_PLAIN, SEGY_INT16,  91, R"( Weathering velocity
    )"},
    {"swevel", SEGY_PLAIN, SEGY_INT16,  93, R"( Subweathering velocity
    )"},
    {"sut", SEGY_PLAIN, SEGY_INT16,  95, R"( Uphole time at source
    )"},
    {"gut", SEGY_PLAIN, SEGY_INT16,  97, R"( Uphole time at receiver group
    )"},
    {"sstat", SEGY_PLAIN, SEGY_INT16,  99, R"( Source static correction
    )"},
    {"gstat", SEGY_PLAIN, SEGY_INT16, 101, R"( Receiver group static correction
    )"},
    {"tstat", SEGY_PLAIN, SEGY_INT16, 103, R"( Total static applied
    )"},
    {"laga", SEGY_PLAIN, SEGY_INT16, 105, R"( Lag time between end of header and time break in milliseconds
    )"},
    {"lagb", SEGY_PLAIN, SEGY_INT16, 107, R"( Lag time between time break and shot in milliseconds
    )"},
    {"delrt", SEGY_PLAIN, SEGY_INT16, 109, R"( Lag time between shot and recording start in milliseconds
    )"},
    {"muts", SEGY_PLAIN, SEGY_INT16, 111, R"( Start of mute time
    )"},
    {"mute", SEGY_PLAIN, SEGY_INT16, 113, R"( End of mute time
    )"},
    {"ns", SEGY_PLAIN, SEGY_INT16, 115, R"( Number of samples in this trace
    )"},
    {"dt", SEGY_PLAIN, SEGY_INT16, 117, R"( Sample interval of this trace in microseconds
    )"},
    {"gain", SEGY_PLAIN, SEGY_INT16, 119, R"( Field instrument gain type code:
        1 = fixed
        2 = binary
        3 = floating point
    )"},
    {"igc", SEGY_PLAIN, SEGY_INT16, 121, R"( Instrument gain constant
    )"},
    {"igi", SEGY_PLAIN, SEGY_INT16, 123, R"( Intrument early gain in decibels
    )"},
    {"corr", SEGY_PLAIN, SEGY_INT16, 125, R"( Correlated (1 = no, 2 = yes)
    )"},
    {"sfs", SEGY_PLAIN, SEGY_INT16, 127, R"( Sweep frequency at start
    )"},
    {"sfe", SEGY_PLAIN, SEGY_INT16, 129, R"( Sweep fequency at end
    )"},
    {"slen", SEGY_PLAIN, SEGY_INT16, 131, R"( Sweep length in milliseconds
    )"},
    {"styp", SEGY_PLAIN, SEGY_INT16, 133, R"( Sweep type code:
        1 = linear
        2 = parabolic
        3 = exponential
        4 = other
    )"},
    {"stas", SEGY_PLAIN, SEGY_INT16, 135, R"( Sweep taper trace length at start in milliseconds
    )"},
    {"stae", SEGY_PLAIN, SEGY_INT16, 137, R"( Sweep taper trace length at end in milliseconds
    )"},
    {"tatyp", SEGY_PLAIN, SEGY_INT16, 139, R"( Taper type code:
        1 = linear
        2 = cosine squared
        3 = other
    )"},
    {"afilf", SEGY_PLAIN, SEGY_INT16, 141, R"( Alias filter frequency
    )"},
    {"afils", SEGY_PLAIN, SEGY_INT16, 143, R"( Alias filter slope
    )"},
    {"nofilf", SEGY_PLAIN, SEGY_INT16, 145, R"( Notch filter frequency
    )"},
    {"nofils", SEGY_PLAIN, SEGY_INT16, 147, R"( Notch filter slope
    )"},
    {"lcf", SEGY_PLAIN, SEGY_INT16, 149, R"( Low cut frequency
    )"},
    {"hcf", SEGY_PLAIN, SEGY_INT16, 151, R"( High cut frequency
    )"},
    {"lcs", SEGY_PLAIN, SEGY_INT16, 153, R"( Low cut slope
    )"},
    {"hcs", SEGY_PLAIN, SEGY_INT16, 155, R"( High cut slope
    )"},
    {"year", SEGY_PLAIN, SEGY_INT16, 157, R"( Year data recorded
    )"},
    {"day", SEGY_PLAIN, SEGY_INT16, 159, R"( Day of year
    )"},
    {"hour", SEGY_PLAIN, SEGY_INT16, 161, R"( Hour of day (24-hour clock)
    )"},
    {"min", SEGY_PLAIN, SEGY_INT16, 163, R"( Minute of hour
    )"},
    {"sec", SEGY_PLAIN, SEGY_INT16, 165, R"( Second of minute
    )"},
    {"timbas", SEGY_PLAIN, SEGY_INT16, 167, R"( Time basis (1 = local, 2 = GMT, 3 = other)
    )"},
    {"trwf", SEGY_PLAIN, SEGY_INT16, 169, R"( Trace weighting factor for fixed-point format data
    )"},
    {"grnors", SEGY_PLAIN, SEGY_INT16, 171, R"( Geophone group number of roll switch position one
    )"},
    {"grnofr", SEGY_PLAIN, SEGY_INT16, 173, R"( Geophone group number of first trace of original field record
    )"},
    {"grnlof", SEGY_PLAIN, SEGY_INT16, 175, R"( Geophone group number of last trace of original field record
    )"},
    {"gaps", SEGY_PLAIN, SEGY_INT16, 177, R"( Gap size (total number of groups dropped)
    )"},
    {"otrav", SEGY_PLAIN, SEGY_INT16, 179, R"( Overtravel associated with taper (1 = down/behind, 2 = up/ahead)
    )"},
    {"cdpx", SEGY_COORD, SEGY_INT32, 181, R"( X cdp coordinate
    )"},
    {"cdpy", SEGY_COORD, SEGY_INT32, 185, R"( Y cdp coordinate
    )"},
    {"iline", SEGY_PLAIN, SEGY_INT32, 189, R"( In-line number
    )"},
    {"xline", SEGY_PLAIN, SEGY_INT32, 193, R"( Cross-line number
    )"},
    {"spno", SEGY_PLAIN, SEGY_INT32, 197, R"( SP number
    )"},
    {"scalsp", SEGY_PLAIN, SEGY_INT16, 201, R"( SP number scalar (applies to #76)
    )"},
    {"unit", SEGY_PLAIN, SEGY_INT16, 203, R"( Trace value measurement unit:
        -1 = Other (should be described in Data Sample Measurement Units Stanza)
        0 = Unknown
        1 = Pascal (Pa)
        2 = Volts (V)
        3 = Millivolts (mV)
        4 = Amperes (A)
        5 = Meters (m)
        6 = Meters per second (m/s)
        7 = Meters per second squared (m/s2)
        8 = Newton (N)
        9 = Watt (W)
    )"},
    {"trc", SEGY_PLAIN, SEGY_BUF6, 205, R"( Transduction constant
    )"},
    {"trunit", SEGY_PLAIN, SEGY_INT16, 211, R"( Transduction Units
        -1 = Other
        0 = Unknown
        1 = Pascal (Pa)
        2 = Volts (V)
        3 = Millivolts (mV)
        4 = Amperes (A)
        5 = Meters (m)
        6 = Meters per second (m/s)
        7 = Meters per second squared (m/s2)
        8 = Newton (N)
        9 = Watt (W)
    )"},
    {"devid", SEGY_PLAIN, SEGY_INT16, 213, R"( Device Identifier
    )"},
    {"scalt", SEGY_PLAIN, SEGY_INT16, 215, R"( Scalar for times and statics (applies to #29-39)
    )"},
    {"stype", SEGY_PLAIN, SEGY_INT16, 217, R"( Source Type/Orientation
        -1 = Other
        0 = Unknown1 = Vibratory - Vertical orientation
        2 = Vibratory - Cross-line orientation
        3 = Vibratory - In-line orientation
        4 = Impulsive - Vertical orientation
        5 = Impulsive - Cross-line orientation
        6 = Impulsive - In-line orientation
        7 = Distributed Impulsive - Vertical orientation
        8 = Distributed Impulsive - Cross-line orientation
        9 = Distributed Impulsive - In-line orientation
    )"},
    {"sdir", SEGY_PLAIN, SEGY_BUF6, 219, R"( Source Energy Direction
    )"},
    {"smeas", SEGY_PLAIN, SEGY_BUF6, 225, R"( Source Measurement
    )"},
    {"sunit", SEGY_PLAIN, SEGY_INT16, 231, R"( Source Measurement Unit
        -1 = Other
        0 = Unknown
        1 = Joule (J)
        2 = Kilowatt (kW)
        3 = Pascal (Pa)
        4 = Bar (Bar)
        4 = Bar-meter (Bar-m)
        5 = Newton (N)
        6 = Kilograms (kg)
    )"},
    {"unass", SEGY_PLAIN, SEGY_BUF6, 233, R"( unassigned - for optional information
    )"}
};
}

#endif // SEGY_TRACEHEADER_DEF_H

