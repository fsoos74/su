#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include <QDialog>
#include <QTimer>

#include <volumeview.h>
#include <grid3d.h>

namespace Ui {
class PlayerDialog;
}

class PlayerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerDialog(QWidget *parent = 0);
    ~PlayerDialog();

    VolumeView::SliceType type();
    int start();
    int stop();
    int step();
    int current();
    int delay();

    Grid3DBounds bounds()const{
        return m_bounds;
    }

public slots:
    void setType(VolumeView::SliceType);
    void setStart(int);
    void setStop(int);
    void setStep(int);
    void setCurrent(int);
    void setDelay(int);
    void setBounds(Grid3DBounds);

    void startAnimation();
    void pauseAnimation();
    void stopAnimation();

signals:
    void sliceRequested(VolumeView::SliceDef);

private slots:
    void on_sbCurrent_valueChanged(int arg1);
    void on_sbStart_valueChanged(int arg1);
    void on_sbStop_valueChanged(int arg1);
    void on_sbStep_valueChanged(int arg1);
    void onTimeout();

    void on_cbType_currentIndexChanged(const QString &arg1);
    void updateControls();
    void enableControls(bool);

    void on_pbStart_clicked();

    void on_pbStop_clicked();

    void on_pbClose_clicked();

private:
    Ui::PlayerDialog *ui;

    QTimer* m_timer;
    bool m_running=false;
    Grid3DBounds m_bounds;
};

#endif // PLAYERDIALOG_H
