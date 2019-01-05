#ifndef CROPDATASETDIALOG_H
#define CROPDATASETDIALOG_H

#include <QDialog>
#include <QMap>
#include <avoproject.h>
#include "processparametersdialog.h"

namespace Ui {
class CropDatasetDialog;
}

class CropDatasetDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit CropDatasetDialog(QWidget *parent = 0);
    ~CropDatasetDialog();

    QMap<QString,QString> params();

public slots:

    void setProject(AVOProject*);
    void setInputDatasets( const QStringList&);
    void setInlineRange(int min, int max);
    void setCrosslineRange(int min, int max);
    void setMSecRange(int min, int max);
    void setOffsetRange(float min, float max);


private slots:
    void updateRanges(QString);
    void updateOkButton();

private:
    Ui::CropDatasetDialog *ui;
    AVOProject* m_project=nullptr;
};

#endif // CropDatasetDialog_H
