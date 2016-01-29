#ifndef PROJECTPROCESS_H
#define PROJECTPROCESS_H

#include <QObject>
#include <memory>

#include <avoproject.h>


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

    ProjectProcess( const QString& name, std::shared_ptr<AVOProject> project, QObject *parent = 0);

    const QString& name()const{
        return m_name;
    }

    std::shared_ptr<AVOProject> project()const{

        return m_project;
    }

    bool isCanceled()const{
        return m_canceled;
    }

    const QString& lastError()const{
        return m_errorString;
    }

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

    std::shared_ptr<AVOProject> m_project;

    bool m_canceled=false;

    QString m_errorString;
};


#endif // PROJECTPROCESS_H
