#ifndef REPLACEBADTRACESPROCESS_H
#define REPLACEBADTRACESPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>
#include <seismicdatasetwriter.h>
#include<memory>

class ReplaceBadTracesProcess : public ProjectProcess
{

    Q_OBJECT

public:

    ReplaceBadTracesProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    std::shared_ptr<seismic::Trace> getTrace( int il, int xl);
    bool isBad( const seismic::Trace& );

    QString m_inputName;
    QString m_outputName;

    std::shared_ptr< SeismicDatasetReader > m_reader;
    std::shared_ptr< SeismicDatasetWriter > m_writer;
};

#endif // ReplaceBadTracesProcess_H
