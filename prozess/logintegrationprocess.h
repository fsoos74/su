#ifndef LOGINTEGRATIONPROCESS_H
#define LOGINTEGRATIONPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<log.h>
#include<memory>


class LogIntegrationProcess : public ProjectProcess
{

    Q_OBJECT

public:

    enum class OP{Running_Sum, Running_Average, Subtract_Previous};
    static QString toQString(OP);
    static OP toOP(QString);
    static QList<OP> ops();
    static QStringList opNames();

    class IOP{
    public:
        virtual void start()=0;
        virtual double operator()(const double&)=0;
        double nullValue()const{
            return m_nullValue;
        }
        void setNullValue(const double& x){
            m_nullValue=x;
        }
    private:
        double m_nullValue;
    };

    LogIntegrationProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

private:

    ProjectProcess::ResultCode processWell(QString well);

    QStringList m_wells;
    QString m_outputName;
    QString m_unit;
    QString m_descr;
    QString m_inputName;

    std::shared_ptr<IOP> m_op;
};


#endif // LOGINTEGRATIONPROCESS_H
