#ifndef VOLUMEDATASELECTIONDIALOG_H
#define VOLUMEDATASELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class VolumeDataSelectionDialog;
}

class VolumeDataSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeDataSelectionDialog(QWidget *parent = 0);
    ~VolumeDataSelectionDialog();

    int minInline()const;
    int maxInline()const;
    int minCrossline()const;
    int maxCrossline()const;
    double minTime()const;
    double maxTime()const;

public slots:

    void setMinInline(int);
    void setMaxInline(int);
    void setMinCrossline(int);
    void setMaxCrossline(int);
    void setMinTime(double);
    void setMaxTime(double);

private slots:

    void checkOk();

private:
    Ui::VolumeDataSelectionDialog *ui;
};

#endif // VOLUMEDATASELECTIONDIALOG_H
