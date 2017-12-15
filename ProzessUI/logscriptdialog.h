#ifndef LOGSCRIPTDIALOG_H
#define LOGSCRIPTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <avoproject.h>


namespace Ui {
class LogScriptDialog;
}

class LogScriptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogScriptDialog(QWidget *parent = 0);
    ~LogScriptDialog();

    QMap<QString,QString> params();

public slots:
    void setProject( AVOProject*);

private slots:
    QStringList selectedWells();
    void updateLogs();
    void updateWells();
    void updateOkButton();
    void updateInputLogControls();

    void on_pbLoad_clicked();
    void on_pbSave_clicked();

private:
    Ui::LogScriptDialog *ui;

    AVOProject* m_project=nullptr;

    QVector<QComboBox*> inputCB;
};

#endif // LOGSCRIPTDIALOG_H
