#ifndef FREQUENCYVOLUMEDIALOG_H
#define FREQUENCYVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"
#include <avoproject.h>

namespace Ui {
class FrequencyVolumeDialog;
}

class FrequencyVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit FrequencyVolumeDialog(QWidget *parent = 0);
    ~FrequencyVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setProject( AVOProject* p);
    void setInputs( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::FrequencyVolumeDialog *ui;

    QStringList m_volumeList;
    AVOProject* m_project=nullptr;
};

#endif // FREQUENCYVOLUMEDIALOG_H
