#ifndef SECONDARYAVOATTRIBUTESDIALOG_H
#define SECONDARYAVOATTRIBUTESDIALOG_H

#include <QDialog>
#include <QMap>

#include "processparametersdialog.h"


namespace Ui {
class SecondaryAVOAttributesDialog;
}

class SecondaryAVOAttributesDialog : public ProcessParametersDialog
{
    Q_OBJECT

public:

    enum TypeIndex{ GxI=0, GqI, GxIxFF };

    explicit SecondaryAVOAttributesDialog(QWidget *parent = 0);
    ~SecondaryAVOAttributesDialog();

    QMap<QString,QString> params();

    bool isVolumeMode()const{
        return m_volumeMode;
    }

public slots:
    void setInterceptList( const QStringList&);
    void setGradientList( const QStringList&);
    void setFluidFactorList( const QStringList&);
    void setVolumeMode(bool);

private slots:
    void updateOkButton();

private:
    Ui::SecondaryAVOAttributesDialog *ui;

    bool m_volumeMode=false;
};

#endif // SECONDARYAVOATTRIBUTESDIALOG_H
