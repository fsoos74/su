#ifndef VOLUMESIMILARITYPROCESS_H
#define VOLUMESIMILARITYPROCESS_H

#include "projectprocess.h"
#include <QObject>

class VolumeSimilarityProcess : public ProjectProcess
{

    Q_OBJECT

public:

    VolumeSimilarityProcess( AVOProject* project, QObject* parent=nullptr);
    ResultCode init(const QMap<QString, QString>& parameters)override;
    ResultCode run()override;

private:

    QString m_volumeName;
    std::shared_ptr<Volume > m_volume;
    std::shared_ptr<Volume > m_input;
    std::shared_ptr<Volume > m_ildip;
    std::shared_ptr<Volume > m_xldip;
};

#endif // VOLUMESIMILARITYPROCESS_H
