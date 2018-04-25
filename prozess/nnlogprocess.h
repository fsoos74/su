#ifndef NNLOGPROCESS_H
#define NNLOGPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>


class NNLogProcess : public ProjectProcess
{

    Q_OBJECT

public:

    NNLogProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_well;
    QStringList m_inputNames;
    QString m_predictedName;
    QString m_outputName;
    //QString m_unit;
    //QString m_descr;

    size_t m_hiddenNeurons;

    unsigned m_trainingEpochs;
    double m_trainingRatio;
    double m_learningRate;

    QString m_paramFileName;
    QString m_historyFileName;

};

#endif // NNLOGPROCESS_H
