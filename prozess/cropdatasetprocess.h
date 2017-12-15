#ifndef CROPDATASETPROCESS_H
#define CROPDATASETPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include <seismicdatasetreader.h>
#include <seismicdatasetwriter.h>
#include<memory>

class CropDatasetProcess : public ProjectProcess
{

    Q_OBJECT

public:

    CropDatasetProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;

    int m_minIline;
    int m_maxIline;
    int m_minXline;
    int m_maxXline;
    int m_minMSec;
    int m_maxMSec;
    float m_minOffset;
    float m_maxOffset;

    std::shared_ptr< SeismicDatasetReader > m_reader;
    std::shared_ptr< SeismicDatasetWriter > m_writer;
};

#endif // CropDatasetProcess_H
