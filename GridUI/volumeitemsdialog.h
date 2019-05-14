#ifndef VOLUMEITEMSDIALOG_H
#define VOLUMEITEMSDIALOG_H

#include <QDialog>
#include <volumeitem.h>
#include <viewitemmodel.h>
#include <avoproject.h>

namespace Ui {
class VolumeItemsDialog;
}

class VolumeItemsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeItemsDialog(AVOProject* project, ViewItemModel* model, QWidget *parent = 0);
    ~VolumeItemsDialog();

private slots:
    void on_lwDisplayed_currentRowChanged(int currentRow);
    void on_pbAdd_clicked();
    void on_pbRemove_clicked();
    void on_pbMoveUp_clicked();
    void on_pbMoveDown_clicked();
    void on_pbColors_clicked();
    void on_pbScaling_clicked();
    void on_pbApply_clicked();

private:
    Ui::VolumeItemsDialog *ui;
    AVOProject* mProject;
    ViewItemModel* mModel=nullptr;
};

#endif // VOLUMEITEMSDIALOG_H
