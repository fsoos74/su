#ifndef LOGSPECTRUMVIEWER_H
#define LOGSPECTRUMVIEWER_H

#include <QMainWindow>
#include <memory>
#include <avoproject.h>
#include <log.h>

namespace Ui {
class LogSpectrumViewer;
}

class LogSpectrumViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogSpectrumViewer(QWidget *parent = 0);
    ~LogSpectrumViewer();

public slots:
    void setProject( AVOProject*);
    void setUWI(const QString&);
    void setLog( std::shared_ptr<Log> );
    void openLog();
    void openLog(QString);

private:
    Ui::LogSpectrumViewer *ui;

    AVOProject* m_project;
    QString m_uwi;
};

#endif // LOGSPECTRUMVIEWER_H
