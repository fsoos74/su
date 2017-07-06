#ifndef FREQUENCYVOLUMEDIALOG_H
#define FREQUENCYVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"


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
    void setInputs( const QStringList&);

private slots:
    void updateOkButton();

private:
    Ui::FrequencyVolumeDialog *ui;

    QStringList m_volumeList;           // need this to check whether volume allready exists
};

#endif // FREQUENCYVOLUMEDIALOG_H
