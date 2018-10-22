#ifndef NNVOLUMECLASSIFICATIONPROCESS_H
#define NNVOLUMECLASSIFICATIONPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<memory>
#include<nn.h>
#include<volumereader2.h>
#include<volumewriter2.h>
#include "simplemlp.h"

class NNVolumeClassificationProcess : public ProjectProcess
{

    Q_OBJECT

public:

    using MLP=SimpleMLP;

    NNVolumeClassificationProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode processWell(QString);

    QString m_inputFile;
    QString m_outputName;
    MLP m_mlp;
    QStringList m_inames;
    std::vector<VolumeReader2*> m_inputVolumeReaders;
};

#endif // NNVOLUMECLASSIFICATIONPROCESS_H
