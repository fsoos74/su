#ifndef BLMFREQUENCIESDIALOG_H
#define BLMFREQUENCIESDIALOG_H

#include <QDialog>
#include<processparametersdialog.h>

namespace Ui {
class BLMFrequenciesDialog;
}

class BLMFrequenciesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit BLMFrequenciesDialog(QWidget *parent = nullptr);
    ~BLMFrequenciesDialog();

    QMap<QString,QString> params();

public slots:
    void setLineName(QString);
    void setExportPath(QString);

private:
    Ui::BLMFrequenciesDialog *ui;
};

#endif // BLMFREQUENCIESDIALOG_H
