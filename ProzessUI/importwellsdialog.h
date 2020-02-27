#ifndef ImportWellDIALOG_H
#define ImportWellDIALOG_H

#include <processparametersdialog.h>


namespace Ui {
class ImportWellsDialog;
}



class ImportWellsDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    const int PreviewLines=100;
    const qint64 MaxLineLength=1024;

    explicit ImportWellsDialog(QWidget *parent = 0);
    ~ImportWellsDialog();

    QMap<QString,QString> params() override;


private slots:

    void on_pbBrowse_clicked();
    void updatePreview( const QString&);
    void updateOkButton();

private:

    Ui::ImportWellsDialog *ui;

};

#endif // ImportWellDIALOG_H
