#ifndef SELECTWELLSBYLOGDIALOG_H
#define SELECTWELLSBYLOGDIALOG_H

#include <QDialog>
#include <avoproject.h>

namespace Ui {
class SelectWellsByLogDialog;
}

class SelectWellsByLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectWellsByLogDialog(QWidget *parent = 0);
    ~SelectWellsByLogDialog();
    QString log();
    QStringList wells();

public slots:
    void setProject( AVOProject* project);


private slots:
    void updateLogs();              // all logs from project
    void updateWells(QString log);  // find wells which have log

private:
    Ui::SelectWellsByLogDialog *ui;

    AVOProject* m_project=nullptr;      // parent must own project and not delete before dialog is finished!!!!

};

#endif // SELECTWELLSBYLOGDIALOG_H
