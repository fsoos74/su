#ifndef TABLEITEMSDIALOG_H
#define TABLEITEMSDIALOG_H

#include <QDialog>
#include <tableitem.h>
#include <viewitemmodel.h>
#include <avoproject.h>

namespace sliceviewer {

namespace Ui {
class TableItemsDialog;
}

class TableItemsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent = 0);
    ~TableItemsDialog();

private slots:
    void on_lwDisplayed_currentRowChanged(int currentRow);
    void on_pbAdd_clicked();
    void on_pbRemove_clicked();
    void on_pbMoveUp_clicked();
    void on_pbMoveDown_clicked();
    void on_cbColor_clicked();
    void on_pbApply_clicked();

private:
    Ui::TableItemsDialog *ui;
    AVOProject* mProject;
    ViewItemModel* mModel=nullptr;
};

}

#endif // TABLEITEMSDIALOG_H
