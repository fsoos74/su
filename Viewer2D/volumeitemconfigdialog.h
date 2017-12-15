#ifndef VOLUMEITEMCONFIGDIALOG_H
#define VOLUMEITEMCONFIGDIALOG_H

#include<griditemconfigdialog.h>
#include <QSpinBox>
#include <volumeitem.h>

class VolumeItemConfigDialog : public GridItemConfigDialog
{
    Q_OBJECT
public:
    VolumeItemConfigDialog(QWidget* parent=nullptr);

    int time();     // msec

signals:
    void timeChanged(double);       // secs

public slots:
    void setTimeRange(int,int);     // msecs
    void setTimeIncrement(int);     // msecs
    void setTime(int);              // msec

private slots:
    void onSBTimeValueChanged(int);

private:
    QSpinBox* m_sbTime; // time in msecs!!!
};

#endif // VOLUMEITEMCONFIGDIALOG_H
