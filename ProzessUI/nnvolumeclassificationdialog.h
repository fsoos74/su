#ifndef NNVOLUMECLASSIFICATIONDIALOG_H
#define NNVOLUMECLASSIFICATIONDIALOG_H

#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>

#include<nn.h>

namespace Ui {
class NNVolumeClassificationDialog;
}

class NNVolumeClassificationDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit NNVolumeClassificationDialog(QWidget *parent = 0);
    ~NNVolumeClassificationDialog();

    QMap<QString,QString> params();

public slots:
    void setProject(AVOProject*);

private slots:
    void on_pbBrowse_clicked();
    void updateOkButton();
    void on_leInputFile_textChanged(const QString &arg1);

private:

    QStringList selectedWells();

    Ui::NNVolumeClassificationDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // NNVOLUMECLASSIFICATIONDIALOG_H
