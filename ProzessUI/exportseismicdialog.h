#ifndef EXPORTSEISMICDIALOG_H
#define EXPORTSEISMICDIALOG_H


#include <QDialog>
#include <processparametersdialog.h>

namespace Ui {
class ExportSeismicDialog;
}

class ExportSeismicDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ExportSeismicDialog(QWidget *parent = 0);
    ~ExportSeismicDialog();

    QMap<QString,QString> params() override;

public slots:
    void setDatasets( const QStringList&);
    void setFixedDataset( const QString&);
    void setMinimumInline( int );
    void setMaximumInline( int );
    void setMinimumCrossline( int );
    void setMaximumCrossline( int );

private slots:
    void on_pbBrowse_clicked();
    void updateOkButton();

private:
    Ui::ExportSeismicDialog *ui;
};

#endif // EXPORTSEISMICDIALOG_H
