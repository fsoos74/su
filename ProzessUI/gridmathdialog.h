#ifndef GRIDMATHDIALOG_H
#define GRIDMATHDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>
#include <avoproject.h>


namespace Ui {
class GridMathDialog;
}

class GridMathDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit GridMathDialog(QWidget *parent = 0);
    ~GridMathDialog();

    QMap<QString,QString> params();


public slots:
    void setProject(AVOProject*);

private slots:
    void on_cbFunction_currentIndexChanged(QString arg1);
    void updateInputGrids();
    void updateOkButton();

private:
    Ui::GridMathDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // GRIDMATHDIALOG_H
