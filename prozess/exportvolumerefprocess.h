#ifndef EXPORTVOLUMEREFPROCESS_H
#define EXPORTVOLUMEREFPROCESS_H


#include "projectprocess.h"
#include <QObject>

class ExportVolumeRefProcess : public ProjectProcess
{

    Q_OBJECT

public:
    ExportVolumeRefProcess(AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters)override;
    ResultCode run()override;

private:

    QString m_outputFilename;
    QString m_volumeName;
    QString m_refStackName;
    float m_nullValue;
    QStringList m_ebcdic;
};


#endif // EXPORTVOLUMEREFPROCESS_H
