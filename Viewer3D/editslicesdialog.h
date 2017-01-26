#ifndef EDITSLICESDIALOG_H
#define EDITSLICESDIALOG_H

#include <QDialog>
#include <QVector>
#include <slicedef.h>
#include <grid3d.h>  // need bounds
#include <QVector>

namespace Ui {
class EditSlicesDialog;
}

class EditSlicesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSlicesDialog(QWidget *parent = 0);
    ~EditSlicesDialog();

private slots:
    void sliceToControls( int );

private:
    Ui::EditSlicesDialog *ui;


};

#endif // EDITSLICESDIALOG_H
