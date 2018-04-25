#ifndef FMCDP2DPROCESS_H
#define FMCDP2DPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <QVector>

class FMCDP2DProcess : public ProjectProcess
{

    Q_OBJECT

public:

    FMCDP2DProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    double m_f0;
    double m_delay;
    double m_dxz;
    double m_tmax;
    double m_offset1;
    double m_offset2;
    unsigned m_nrc;
    bool m_split_spread;
    QString m_model;        // layers
    QString m_outputName;
};

#endif // FMCDP2DPROCESS_H
