#ifndef STACKTOGATHERPROCESS_H
#define STACKTOGATHERPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include <seismicdatasetreader.h>
#include <seismicdatasetwriter.h>
#include<memory>

class StackToGatherProcess : public ProjectProcess
{

    Q_OBJECT

public:

    StackToGatherProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    QString m_outputName;
    QStringList m_inputNames;
    QVector<double> m_inputValues;

};

#endif // StackToGatherProcess_H
