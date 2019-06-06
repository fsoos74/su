#ifndef WELLITEMSDIALOG_H
#define WELLITEMSDIALOG_H

#include <QDialog>
#include <wellitem.h>
#include <viewitemmodel.h>
#include <avoproject.h>

namespace sliceviewer {

namespace Ui {
class WellItemsDialog;
}

class WellItemsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WellItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent = 0);
    ~WellItemsDialog();

private slots:
    void on_lwDisplayed_currentRowChanged(int currentRow);
    void on_pbAdd_clicked();
    void on_pbRemove_clicked();
    void on_pbMoveUp_clicked();
    void on_pbMoveDown_clicked();
    void on_cbColor_clicked();
    void on_pbApply_clicked();

private:
    Ui::WellItemsDialog *ui;
    AVOProject* mProject;
    ViewItemModel* mModel=nullptr;
};

}

#endif // WELLITEMSDIALOG_H
