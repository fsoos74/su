#ifndef EXPORTVOLUMEREFDIALOG_H
#define EXPORTVOLUMEREFDIALOG_H

#include <QDialog>
#include <processparametersdialog.h>
class AVOProject;

namespace Ui {
class ExportVolumeRefDialog;
}

class ExportVolumeRefDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ExportVolumeRefDialog(QWidget *parent = 0);
    ~ExportVolumeRefDialog();
    void setProject(AVOProject*);

    QMap<QString,QString> params() override;

public slots:


private slots:
    void on_pbBrowse_clicked();
    void updateOkButton();


private:
    Ui::ExportVolumeRefDialog *ui;
    AVOProject* mProject;
};

#endif // EXPORTVOLUMEREFDIALOG_H
