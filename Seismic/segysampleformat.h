#ifndef SAMPLEFORMAT_H
#define SAMPLEFORMAT_H

#include<QString>
#include<QMap>
#include<cstdint>
namespace seismic{

enum class SEGYSampleFormat{ IBM, INT4, INT2, FIXP, IEEE, INT1 };

struct SEGYSampleFormatInfo{

    QString         name;
    std::int16_t    code;
    size_t          size;
};

static const QMap<SEGYSampleFormat, SEGYSampleFormatInfo> SampleFormatInfo{

    {SEGYSampleFormat::IBM,  {"IBM", 1, 4} },
    {SEGYSampleFormat::INT4, {"INT4", 2, 4} },
    {SEGYSampleFormat::INT2, {"INT2", 3, 2} },
    {SEGYSampleFormat::INT1, {"INT1", 8, 1} },
    {SEGYSampleFormat::FIXP, {"FIXP", 4, 4} },
    {SEGYSampleFormat::IEEE, {"IEEE", 5, 4} }

};


QString toQString( SEGYSampleFormat );
SEGYSampleFormat toSampleFormat( const QString&);

std::int16_t toInt( SEGYSampleFormat);
SEGYSampleFormat toSampleFormat(std::int16_t);

size_t bytesPerSample( SEGYSampleFormat);


}

#endif // SAMPLEFORMAT_H

