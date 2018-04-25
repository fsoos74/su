#ifndef EXPORTVOLUMEPROCESS_H
#define EXPORTVOLUMEPROCESS_H


#include "projectprocess.h"
#include <QObject>

#include<segy_text_header.h>
#include<header.h>

class ExportVolumeProcess : public ProjectProcess
{

    Q_OBJECT

public:
    ExportVolumeProcess(AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters)override;
    ResultCode run()override;

private:

    seismic::SEGYTextHeader buildTextHeader()const;
    seismic::Header buildBinaryHeader(size_t dt_us, size_t ns)const;

    QString m_outputFilename;
    QString m_volumeName;
    float m_nullValue;

    int m_minInline;
    int m_maxInline;
    int m_minCrossline;
    int m_maxCrossline;
    float m_minTime;
    float m_maxTime;

    std::shared_ptr<Grid3D<float>> m_volume;

    QTransform m_IlXlToXY;
    QTransform m_XYToIlXl;
    unsigned m_ns;
    unsigned m_dt_us;

};


#endif // EXPORTVOLUMEPROCESS_H
