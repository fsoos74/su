#ifndef VOLUMESIMILARITYDIALOG_H
#define VOLUMESIMILARITYDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class VolumeSimilarityDialog;
}

class VolumeSimilarityDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit VolumeSimilarityDialog(QWidget *parent = 0);
    ~VolumeSimilarityDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:
    void updateInputVolumes();
    void updateOkButton();

private:
    Ui::VolumeSimilarityDialog *ui;

    AVOProject* m_project;
};

#endif // VOLUMESIMILARITYDIALOG_H
