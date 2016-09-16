#ifndef EXPORTVOLUMEDIALOG_H
#define EXPORTVOLUMEDIALOG_H

#include <QDialog>
#include <processparametersdialog.h>

namespace Ui {
class ExportVolumeDialog;
}

class ExportVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit ExportVolumeDialog(QWidget *parent = 0);
    ~ExportVolumeDialog();

    QMap<QString,QString> params() override;

public slots:
    void setVolumes( const QStringList&);
    void setFixedVolume( const QString&);
    void setMinimumInline( int );
    void setMaximumInline( int );
    void setMinimumCrossline( int );
    void setMaximumCrossline( int );
    void setMinimumTime(double );
    void setMaximumTime( double );

private slots:
    void on_pbBrowse_clicked();
    void updateOkButton();


private:
    Ui::ExportVolumeDialog *ui;
};

#endif // EXPORTVOLUMEDIALOG_H
