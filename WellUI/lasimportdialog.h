#ifndef LASIMPORTDIALOG_H
#define LASIMPORTDIALOG_H

#include <QDialog>
#include <QVector>
#include <las_data.h>
#include <avoproject.h>


namespace Ui {
class LASImportDialog;
}

class LASImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LASImportDialog(QWidget *parent = 0);
    ~LASImportDialog();

    const well::LASData& data()const{       // possibly dangerous BUT efficient
        return m_data;
    }

    QVector<int> selectedCurves();

    static void import( AVOProject* );
    static void bulkImport( AVOProject* );

private slots:
    void on_pbBrowse_clicked();

private:

    void readLAS(QString);
    void updateWellInformation();
    void updateCurvesList();

    Ui::LASImportDialog *ui;

    well::LASData m_data;
};

#endif // LASIMPORTDIALOG_H
