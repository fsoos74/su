#ifndef EXPORTTABLEDIALOG_H
#define EXPORTTABLEDIALOG_H

#include <QDialog>
#include <processparametersdialog.h>

namespace Ui {
class ExportTableDialog;
}

class ExportTableDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ExportTableDialog(QWidget *parent = 0);
    ~ExportTableDialog();

    QMap<QString,QString> params() override;

public slots:
    void setTables( const QStringList&);
    void setFixedTable(const QString&);

private slots:
    void on_pbBrowse_clicked();
    void updateOkButton();


private:
    Ui::ExportTableDialog *ui;
};

#endif // EXPORTTABLEDIALOG_H
