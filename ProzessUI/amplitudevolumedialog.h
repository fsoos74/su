#ifndef AMPLITUDEVOLUMEDIALOG_H
#define AMPLITUDEVOLUMEDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"


namespace Ui {
class AmplitudeVolumeDialog;
}

class AmplitudeVolumeDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:
    explicit AmplitudeVolumeDialog(QWidget *parent = 0);
    ~AmplitudeVolumeDialog();

    QMap<QString,QString> params();

public slots:
    void setDatasets( const QStringList&);
    void setWindowStartMS( int );
    void setWindowEndMS( int );

private slots:
    void updateOkButton();

private:
    Ui::AmplitudeVolumeDialog *ui;

    QStringList m_volumeList;           // need this to check whether volume allready exists
};

#endif // AMPLITUDEVOLUMEDIALOG_H
