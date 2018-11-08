#ifndef MERGEVOLUMESDIALOG_H
#define MERGEVOLUMESDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"


namespace Ui {
class MergeVolumesDialog;
}

class MergeVolumesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    enum OverlapIndex{ Volume1=0, Volume2=1, Mean=2 };

    explicit MergeVolumesDialog(QWidget *parent = 0);
    ~MergeVolumesDialog();

    QMap<QString,QString> params();

public slots:
    void setInputVolumes( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::MergeVolumesDialog *ui;
};

#endif // MERGEVOLUMESDIALOG_H
