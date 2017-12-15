#ifndef IMPORTGRIDDIALOG_H
#define IMPORTGRIDDIALOG_H

#include <avoproject.h>
#include <gridformat.h>
#include <processparametersdialog.h>


namespace Ui {
class ImportGridDialog;
}



class ImportGridDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    enum class Format{ XYZ, ILXLZ };

    const int PreviewLines=100;
    const qint64 MaxLineLength=1024;

    explicit ImportGridDialog(QWidget *parent = 0);
    ~ImportGridDialog();

    QMap<QString,QString> params() override;

public slots:

    void setProject(AVOProject*);
    void setType(GridType);
    void setFormat( QString );
    void setFormat( GridFormat );

    void setCoord1Column( int );
    void setCoord2Column( int );
    void setValueColumn( int );
    void setNullValue( const QString& );

private slots:

    void on_pbBrowse_clicked();
    void updatePreview( const QString&);
    void scan();
    void on_pbScan_clicked();
    void updateOkButton();

private:

    bool splitLine( const QString& line, qreal& coord1, qreal& coord2, double& value);
    void updateLastUsedColumn();
    void updateCoordLabels();

    Ui::ImportGridDialog *ui;

    AVOProject* m_project=nullptr;

    GridFormat m_format;
    int m_coord1Column=0;
    int m_coord2Column=1;
    int m_valueColumn=2;
    int m_lastUsedColumn=2;
    QString m_nullValue="-999.999";
};

#endif // ImportGridDialog_H
