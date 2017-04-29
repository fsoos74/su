#ifndef GRIDMATHDIALOG_H
#define GRIDMATHDIALOG_H

#include <QDialog>
#include <QMap>
#include <processparametersdialog.h>

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
    void setInputGrids( QString, const QStringList&);

private slots:
    void on_cbFunction_currentIndexChanged(int index);
    void updateInputGrids();
   // void updateInputGridControls();
    void updateOkButton();

private:
    Ui::GridMathDialog *ui;

    QMap<QString, QStringList> m_inputGrids;

};

#endif // GRIDMATHDIALOG_H
