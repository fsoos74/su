#ifndef GRIDRUNUSERSCRIPTDIALOG_H
#define GRIDRUNUSERSCRIPTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>

namespace Ui {
class GridRunUserScriptDialog;
}

class GridRunUserScriptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridRunUserScriptDialog(QWidget *parent = 0);
    ~GridRunUserScriptDialog();

    QMap<QString,QString> params();

public slots:
    void setInputGrids( const QStringList&);

private slots:
    void updateInputGridControls();
    void updateOkButton();

    void on_pbLoad_clicked();

    void on_pbSave_clicked();

private:
    Ui::GridRunUserScriptDialog *ui;

    QStringList m_inputGrids;

    QVector<QComboBox*> inputCB;
};

#endif // GRIDRUNUSERSCRIPTDIALOG_H
