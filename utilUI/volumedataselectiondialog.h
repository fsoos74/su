#ifndef VOLUMEDATASELECTIONDIALOG_H
#define VOLUMEDATASELECTIONDIALOG_H

#include <QDialog>

#include <volumedimensions.h>

namespace Ui {
class VolumeDataSelectionDialog;
}

class VolumeDataSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeDataSelectionDialog(QWidget *parent = 0);
    ~VolumeDataSelectionDialog();

    bool isAreaMode(){
        return m_areaMode;
    }

    int minInline()const;
    int maxInline()const;
    int minCrossline()const;
    int maxCrossline()const;
    int minMSec()const;
    int maxMSec()const;
    VolumeDimensions dimensions()const;

public slots:

    void setAreaMode( bool );

    void setMinInline(int);
    void setMaxInline(int);
    void setMinCrossline(int);
    void setMaxCrossline(int);
    void setMinMSec(int);
    void setMaxMSec(int);
    void setDimensions( VolumeDimensions );
    void setInlineSelectionEnabled(bool);
    void setCrosslineSelectionEnabled(bool);
    void setMSecSelectionEnabled(bool);

signals:
    void areaModeChanged(bool);

private slots:

    void checkOk();

private:
    Ui::VolumeDataSelectionDialog *ui;

    bool m_areaMode=false;
};

#endif // VOLUMEDATASELECTIONDIALOG_H
