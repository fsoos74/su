#ifndef OFFSETCOMPUTATIONPROCESS_H
#define OFFSETCOMPUTATIONPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include <seismicdatasetreader.h>
#include <seismicdatasetwriter.h>
#include<memory>

class OffsetComputationProcess : public ProjectProcess
{

    Q_OBJECT

public:

    OffsetComputationProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;

    std::shared_ptr< SeismicDatasetReader > m_reader;
    std::shared_ptr< SeismicDatasetWriter > m_writer;
};

#endif // OffsetComputationProcess_H
