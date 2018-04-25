#ifndef NNLOGINTERPOLATIONDIALOG_H
#define NNLOGINTERPOLATIONDIALOG_H

#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>

#include<nn.h>

namespace Ui {
class NNLogInterpolationDialog;
}

class NNLogInterpolationDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit NNLogInterpolationDialog(QWidget *parent = 0);
    ~NNLogInterpolationDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:
    void on_pbBrowse_clicked();
    void updateOkButton();
    void updateInputWells(QStringList requiredLogs);
    void on_leInputFile_textChanged(const QString &arg1);

private:

    QStringList selectedWells();

    Ui::NNLogInterpolationDialog *ui;

    AVOProject* m_project=nullptr;
    QStringList m_welllogs;
};

#endif // NNLOGINTERPOLATIONDIALOG_H
