#ifndef CONVERTLOGDEPTHTIMEDIALOG_H
#define CONVERTLOGDEPTHTIMEDIALOG_H

#include <QDialog>

namespace Ui {
class ConvertLogDepthTimeDialog;
}

class ConvertLogDepthTimeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvertLogDepthTimeDialog(QWidget *parent = 0);
    ~ConvertLogDepthTimeDialog();

    int start();
    int stop();
    int interval();
    qreal depth0();
    QString velocityLog();

public slots:
    void setStart(int);
    void setStop(int);
    void setInterval(int);
    void setDepth0(qreal);
    void setVelocityLog(QString);
    void setVelocityLogs(QStringList);

private slots:
    void updateOkButton();

private:
    Ui::ConvertLogDepthTimeDialog *ui;
};

#endif // CONVERTLOGDEPTHTIMEDIALOG_H
