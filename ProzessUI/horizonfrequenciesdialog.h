#ifndef HORIZONFREQUENCIESDIALOG_H
#define HORIZONFREQUENCIESDIALOG_H

#include <QDialog>
#include "processparametersdialog.h"


namespace Ui {
class HorizonFrequenciesDialog;
}

class HorizonFrequenciesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit HorizonFrequenciesDialog(QWidget *parent = 0);
    ~HorizonFrequenciesDialog();

    QMap<QString,QString> params();

public slots:
    void setDatasets( const QStringList&);
    void setHorizons( const QStringList&);


private slots:
    void updateOkButton();

private:
    Ui::HorizonFrequenciesDialog *ui;
};

#endif // HorizonFrequenciesDialog_H
