#ifndef GRIDRUNUSERSCRIPTDIALOG_H
#define GRIDRUNUSERSCRIPTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QMap>

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
    void setInputGrids( QString, const QStringList&);

private slots:
    void updateInputGrids();
    void updateInputGridControls();
    void updateOkButton();

    void on_pbLoad_clicked();

    void on_pbSave_clicked();

private:
    Ui::GridRunUserScriptDialog *ui;

    QMap<QString, QStringList> m_inputGrids;

    QVector<QComboBox*> inputCB;
};

#endif // GRIDRUNUSERSCRIPTDIALOG_H
