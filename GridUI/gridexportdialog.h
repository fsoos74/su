#ifndef GRIDEXPORTDIALOG_H
#define GRIDEXPORTDIALOG_H

#include <QDialog>
#include<QCloseEvent>
#include <grid2d.h>
#include <memory>


namespace Ui {
class GridExportDialog;
}

class GridExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridExportDialog(std::shared_ptr<Grid2D<double>>, QWidget *parent = 0);
    ~GridExportDialog();

private slots:
    void on_pushButton_clicked();

    void on_leFile_textChanged(const QString &arg1);

    void on_pbExport_clicked();

private:

    bool exportGrid();

    Ui::GridExportDialog *ui;

    std::shared_ptr<Grid2D<double>> m_grid;
};

#endif // GRIDEXPORTDIALOG_H
