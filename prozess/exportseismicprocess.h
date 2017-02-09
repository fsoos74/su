#ifndef EXPORTSEISMICPROCESS_H
#define EXPORTSEISMICPROCESS_H

#include "projectprocess.h"
#include <QObject>

#include<seismicdatasetreader.h>


class ExportSeismicProcess : public ProjectProcess
{

    Q_OBJECT

public:
    ExportSeismicProcess(AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters)override;
    ResultCode run()override;

private:

    QString m_outputFilename;
    QString m_datasetName;

    int m_minInline;
    int m_maxInline;
    int m_minCrossline;
    int m_maxCrossline;

    std::shared_ptr<SeismicDatasetReader> m_reader;
};

#endif // EXPORTSEISMICPROCESS_H
