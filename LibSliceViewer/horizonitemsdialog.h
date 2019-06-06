#ifndef HORIZONITEMSDIALOG_H
#define HORIZONITEMSDIALOG_H

#include <QDialog>
#include <horizonitem.h>
#include <viewitemmodel.h>
#include <avoproject.h>


namespace sliceviewer {

namespace Ui {
class HorizonItemsDialog;
}

class HorizonItemsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HorizonItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent = 0);
    ~HorizonItemsDialog();

private slots:
    void on_lwDisplayed_currentRowChanged(int currentRow);
    void on_pbAdd_clicked();
    void on_pbRemove_clicked();
    void on_pbMoveUp_clicked();
    void on_pbMoveDown_clicked();
    void on_cbColor_clicked();
    void on_pbApply_clicked();

private:
    Ui::HorizonItemsDialog *ui;
    AVOProject* mProject;
    ViewItemModel* mModel=nullptr;
};

}

#endif // HORIZONITEMSDIALOG_H
