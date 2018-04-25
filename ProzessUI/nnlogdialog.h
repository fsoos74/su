#ifndef NNLOGDIALOG_H
#define NNLOGDIALOG_H

#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class NNLogDialog;
}

class NNLogDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit NNLogDialog(QWidget *parent = 0);
    ~NNLogDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:
    void on_pbParamBrowse_clicked();
    void on_pbHistBrowse_clicked();
    void updateInputAndPredicted(QString);
    void updateOkButton();

private:

    QStringList selectedLogs();

    Ui::NNLogDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // NNLOGDIALOG_H
