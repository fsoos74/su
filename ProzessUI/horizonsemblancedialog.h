#ifndef HORIZONSEMBLANCEDIALOG_H
#define HORIZONSEMBLANCEDIALOG_H

#include <QDialog>
#include<QMap>

#include "processparametersdialog.h"

namespace Ui {
class HorizonSemblanceDialog;
}

class HorizonSemblanceDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit HorizonSemblanceDialog(QWidget *parent = 0);
    ~HorizonSemblanceDialog();

    QMap<QString,QString> params();

public slots:
    void setDatasets( const QStringList&);
    void setHorizons( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::HorizonSemblanceDialog *ui;
};

#endif // HORIZONSEMBLANCEDIALOG_H
