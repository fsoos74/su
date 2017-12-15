#ifndef LOGMATHDIALOG_H
#define LOGMATHDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class LogMathDialog;
}

class LogMathDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit LogMathDialog(QWidget *parent = 0);
    ~LogMathDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:
    void on_cbFunction_currentIndexChanged(QString arg1);
    QStringList selectedWells();
    void updateLogs();
    void updateWells();
    void updateOkButton();

private:
    Ui::LogMathDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // LOGMATHDIALOG_H
