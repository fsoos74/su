#ifndef ADDSLICEDIALOG_H
#define ADDSLICEDIALOG_H

#include <QDialog>

#include<slicedef.h>

namespace Ui {
class AddSliceDialog;
}

class AddSliceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSliceDialog(QWidget *parent = 0);
    ~AddSliceDialog();

    SliceDef sliceDef();

public slots:

    void setSliceDef( SliceDef );

private:
    Ui::AddSliceDialog *ui;
};

#endif // ADDSLICEDIALOG_H
