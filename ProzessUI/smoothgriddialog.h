#ifndef SmoothGridDialog_H
#define SmoothGridDialog_H

#include <QDialog>
#include<QMap>

#include "processparametersdialog.h"

namespace Ui {
class SmoothGridDialog;
}

class SmoothGridDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit SmoothGridDialog(QWidget *parent = 0);
    ~SmoothGridDialog();

    QMap<QString,QString> params();

public slots:
    void setInputGrids( QString type, const QStringList& l);

private slots:
    void updateInputGrids();
    void updateOkButton();

private:
    Ui::SmoothGridDialog *ui;
    QMap<QString, QStringList> m_inputGrids;
};

#endif // SmoothGridDialog_H
