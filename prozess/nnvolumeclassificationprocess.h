#ifndef NNVOLUMECLASSIFICATIONPROCESS_H
#define NNVOLUMECLASSIFICATIONPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<memory>
#include<nn.h>
#include<volumereader2.h>
#include<volumewriter2.h>

class NNVolumeClassificationProcess : public ProjectProcess
{

    Q_OBJECT

public:

    NNVolumeClassificationProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode processWell(QString);

    QString m_inputFile;
    QString m_outputName;
    NN m_nn;
    QStringList m_inames;
    int m_apertureLines;
    std::vector<std::pair<double,double>> m_Xmm;
    std::pair<double,double> m_Ymm;
    std::vector<VolumeReader2*> m_inputVolumeReaders;
};

#endif // NNVOLUMECLASSIFICATIONPROCESS_H
