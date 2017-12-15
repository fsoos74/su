#ifndef PROJECTPROCESS_H
#define PROJECTPROCESS_H

#include <QObject>
#include <memory>

#include <avoproject.h>
#include <processparams.h>

class ProjectProcess : public QObject
{
    Q_OBJECT

public:

    enum class ResultCode{ Ok, Canceled, Error };

    class ParamException : public std::runtime_error{
    public:
        ParamException( const QString& msg):std::runtime_error(msg.toStdString()){

        }
    };

    ProjectProcess( const QString& name, AVOProject* project, QObject *parent = 0);

    const QString& name()const{
        return m_name;
    }

    const ProcessParams& params()const{
        return m_params;
    }

    AVOProject* project()const{

        return m_project;
    }

    bool isCanceled()const{
        return m_canceled;
    }

    const QString& lastError()const{
        return m_errorString;
    }

    void setParams( const ProcessParams&);

    virtual ResultCode init(const QMap<QString, QString>& parameters)=0;
    virtual ResultCode run() = 0;


signals:

    void currentTask( QString );
    void started( int nsteps);
    void finished();
    void progress(int);
    void error(QString);

public slots:

    void cancel();

protected:

    void setErrorString( const QString&);

private:

    QString m_name;

    ProcessParams m_params;

    AVOProject* m_project;

    bool m_canceled=false;

    QString m_errorString;
};


#endif // PROJECTPROCESS_H
