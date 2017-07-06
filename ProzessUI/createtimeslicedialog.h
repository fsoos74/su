#ifndef CREATESLICEDIALOG_H
#define CREATESLICEDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"

namespace Ui {
class CreateTimesliceDialog;
}

class CreateTimesliceDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CreateTimesliceDialog(QWidget *parent = 0);
    ~CreateTimesliceDialog();

    QMap<QString,QString> params();

public slots:
    void setInputs( const QStringList&);
    void setInputLabel( const QString& );
    void setHorizons( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::CreateTimesliceDialog *ui;
};

#endif // CREATESLICEDIALOG_H
