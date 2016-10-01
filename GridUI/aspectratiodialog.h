#ifndef ASPECTRATIODIALOG_H
#define ASPECTRATIODIALOG_H

#include <QDialog>

namespace Ui {
class AspectRatioDialog;
}

class AspectRatioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AspectRatioDialog(QWidget *parent = 0);
    ~AspectRatioDialog();

    double defaultRatio()const{
        return m_defaultRatio;
    }

    double ratio();

    bool isFixed();

public slots:

    void setDefaultRatio(double);
    void setRatio(double);
    void setFixed(bool);

signals:
    void ratioChanged(double);
    void fixedChanged(bool);

private slots:
    void on_pbDefault_clicked();

    void on_cbFixed_toggled(bool checked);


    void on_leRatio_returnPressed();

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::AspectRatioDialog *ui;

    double m_defaultRatio=1;
};

#endif // ASPECTRATIODIALOG_H
