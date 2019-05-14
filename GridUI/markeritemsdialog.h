#ifndef MARKERITEMSDIALOG_H
#define MARKERITEMSDIALOG_H

#include <QDialog>
#include <markeritem.h>
#include <viewitemmodel.h>
#include <avoproject.h>

namespace Ui {
class MarkerItemsDialog;
}

class MarkerItemsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MarkerItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent = 0);
    ~MarkerItemsDialog();

private slots:
    void on_lwDisplayed_currentRowChanged(int currentRow);
    void on_pbAdd_clicked();
    void on_pbRemove_clicked();
    void on_pbMoveUp_clicked();
    void on_pbMoveDown_clicked();
    void on_cbColor_clicked();
    void on_pbApply_clicked();

private:
    Ui::MarkerItemsDialog *ui;
    AVOProject* mProject;
    ViewItemModel* mModel=nullptr;
};

#endif // MARKERITEMSDIALOG_H
