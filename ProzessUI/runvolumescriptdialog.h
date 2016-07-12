#ifndef RUNVOLUMESCRIPTDIALOG_H
#define RUNVOLUMESCRIPTDIALOG_H

#include <QDialog>
#include <QVector>
#include <QComboBox>

namespace Ui {
class RunVolumeScriptDialog;
}

class RunVolumeScriptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunVolumeScriptDialog(QWidget *parent = 0);
    ~RunVolumeScriptDialog();

    QMap<QString,QString> params();

public slots:
    void setInputVolumes( const QStringList&);
    void setInputGrids( const QStringList&);

private slots:
    void updateInputGridControls();
    void updateInputVolumeControls();
    void updateOkButton();

    void on_pbLoad_clicked();

    void on_pbSave_clicked();

private:
    Ui::RunVolumeScriptDialog *ui;

    QStringList m_inputVolumes;
    QStringList m_inputGrids;

    QVector<QComboBox*> inputVolumeCB;
    QVector<QComboBox*> inputGridCB;
};


#endif // RUNVOLUMESCRIPTDIALOG_H
