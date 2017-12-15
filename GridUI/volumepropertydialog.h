#ifndef VOLUMEPROPERTYDIALOG_H
#define VOLUMEPROPERTYDIALOG_H

#include <QDialog>
#include<domain.h>
#include<volumetype.h>
namespace Ui {
class VolumePropertyDialog;
}

class VolumePropertyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VolumePropertyDialog(QWidget *parent = 0);
    ~VolumePropertyDialog();

    Domain domain();
    VolumeType type();
    qreal z0();
    qreal dz();

public slots:
    void setDomain(Domain);
    void setType(VolumeType);
    void setZ0(qreal);
    void setDZ(qreal);

private:
    Ui::VolumePropertyDialog *ui;
};

#endif // VOLUMEPROPERTYDIALOG_H
