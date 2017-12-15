#ifndef EXTRACTDATASETPROCESS_H
#define EXTRACTDATASETPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include <seismicdatasetreader.h>   // this gives qApp
#include <seismicdatasetwriter.h>
#include<memory>

class ExtractDatasetProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ExtractDatasetProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;
    int m_il1, m_il2, m_xl1, m_xl2, m_msec1, m_msec2;

    std::shared_ptr<Grid3D<float> > m_inputVolume;
    std::shared_ptr< SeismicDatasetWriter > m_writer;
};

#endif // EXTRACTDATASETPROCESS_H
