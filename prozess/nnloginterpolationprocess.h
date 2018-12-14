#ifndef NNLOGINTERPOLATIONPROCESS_H
#define NNLOGINTERPOLATIONPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>

#include<simplemlp.h>


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
    SimpleMLP m_mlp;
    QStringList m_inames;
};

#endif // NNLOGINTERPOLATIONPROCESS_H
