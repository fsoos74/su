#ifndef NNLOGINTERPOLATIONPROCESS_H
#define NNLOGINTERPOLATIONPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>

#include<nn.h>


class NNLogInterpolationProcess : public ProjectProcess
{

    Q_OBJECT

public:

    NNLogInterpolationProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ResultCode processWell(QString);

    QString m_inputFile;
    QStringList m_wells;
    QString m_outputName;
    NN m_nn;
    QStringList m_inames;
    std::vector<std::pair<double,double>> m_Xmm;
    std::pair<double,double> m_Ymm;
};

#endif // NNLOGINTERPOLATIONPROCESS_H
