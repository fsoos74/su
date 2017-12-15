#ifndef OFFSETSTACKPROCESS_H
#define OFFSETSTACKPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include <seismicdatasetreader.h>
#include <seismicdatasetwriter.h>
#include<memory>

class OffsetStackProcess : public ProjectProcess
{

    Q_OBJECT

public:

    OffsetStackProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_inputName;
    QString m_outputName;

    float m_minOffset;
    float m_maxOffset;

    std::shared_ptr< SeismicDatasetReader > m_reader;
    std::shared_ptr< SeismicDatasetWriter > m_writer;
};

#endif // OffsetStackProcess_H
