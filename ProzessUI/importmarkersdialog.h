#ifndef IMPORTMARKERSDIALOG_H
#define IMPORTMARKERSDIALOG_H

#include <processparametersdialog.h>


namespace Ui {
class ImportMarkersDialog;
}



class ImportMarkersDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    const int PreviewLines=100;
    const qint64 MaxLineLength=1024;

    explicit ImportMarkersDialog(QWidget *parent = 0);
    ~ImportMarkersDialog();

    QMap<QString,QString> params() override;

public slots:

    void setUWIColumn( int );
    void setNameColumn( int );
    void setMDColumn( int );
    void setSeparator(QString);

private slots:

    void on_pbBrowse_clicked();
    void updatePreview( const QString&);
    void updateOkButton();

private:

    Ui::ImportMarkersDialog *ui;

    QString m_separator;
};

#endif // ImportMarkersDialog_H
