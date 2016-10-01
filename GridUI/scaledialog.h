#ifndef SCALEDIALOG_H
#define SCALEDIALOG_H

#include <QDialog>

namespace Ui {
class ScaleDialog;
}

class ScaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScaleDialog(QWidget *parent = 0);
    ~ScaleDialog();

    bool isHorizontalFixed();
    bool isVerticalFixed();
    int horizontalIncrement();
    int verticalIncrement();

signals:

    void horizontalFixedChanged(bool);
    void verticalFixedChanged(bool);
    void horizontalIncrementChanged(int);
    void verticalIncrementChanged(int);

public slots:

    void setHorizontalFixed(bool);
    void setVerticalFixed(bool);
    void setHorizontalIncrement(int);
    void setVerticalIncrement(int);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);


private slots:
    void on_leHIncrement_returnPressed();
    void on_cbHFixed_toggled(bool checked);
    void on_leVIncrement_returnPressed();
    void on_cbVFixed_toggled(bool checked);

private:
    Ui::ScaleDialog *ui;
};

#endif // SCALEDIALOG_H
