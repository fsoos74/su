#ifndef LOGEDITDIALOG_H
#define LOGEDITDIALOG_H

#include <QDialog>
#include <memory>
#include <avoproject.h>
#include <log.h>


namespace Ui {
class LogEditDialog;
}

class LogEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogEditDialog(QWidget *parent = 0);
    ~LogEditDialog();

    const Log& log();

public slots:
    void setLog(const Log& log);

private:

    void log2model();
    void model2log();

    Ui::LogEditDialog *ui;

    AVOProject* m_project=nullptr;
    QString m_uwi;
    Log m_log;
};


#endif // LOGEDITDIALOG_H
