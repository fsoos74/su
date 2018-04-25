#ifndef POINTDISPLAYOPTIONSDIALOG_H
#define POINTDISPLAYOPTIONSDIALOG_H

#include <QDialog>
#include<QColor>

namespace Ui {
class PointDisplayOptionsDialog;
}

class PointDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PointDisplayOptionsDialog(QWidget *parent = 0);
    ~PointDisplayOptionsDialog();

    int pointSize()const;
    QColor pointColor()const;
    int opacity()const;

signals:

    void pointSizeChanged(int);
    void pointColorChanged( QColor );
    void opacityChanged(int);

public slots:

    void setPointSize(int);
    void setPointColor(QColor);
    void setOpacity(int);

private slots:

    void on_cbColor_clicked();
    void on_sbSize_valueChanged(int arg1);
    void on_sbOpacity_valueChanged(int arg1);

private:
    Ui::PointDisplayOptionsDialog *ui;
};

#endif // POINTDISPLAYOPTIONSDIALOG_H
