#ifndef OVERLAYPERCENTAGEDIALOG_H
#define OVERLAYPERCENTAGEDIALOG_H

#include <QDialog>

namespace Ui {
class OverlayPercentageDialog;
}

class OverlayPercentageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OverlayPercentageDialog(QWidget *parent = 0);
    ~OverlayPercentageDialog();

    int percentage();

public slots:

    void setPercentage(int);

signals:

    void percentageChanged(int);

private slots:
    void on_slPercentage_valueChanged(int value);

private:
    Ui::OverlayPercentageDialog *ui;
};

#endif // OVERLAYPERCENTAGEDIALOG_H
