#ifndef LINEDISPLAYOPTIONSDIALOG_H
#define LINEDISPLAYOPTIONSDIALOG_H

#include <QDialog>
#include<QColor>

namespace Ui {
class LineDisplayOptionsDialog;
}

class LineDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LineDisplayOptionsDialog(QWidget *parent = 0);
    ~LineDisplayOptionsDialog();

    int lineWidth()const;
    QColor color()const;
    int opacity()const;

signals:

    void lineWidthChanged(int);
    void colorChanged( QColor );
    void opacityChanged(int);

public slots:

    void setLineWidth(int);
    void setColor(QColor);
    void setOpacity(int);

private slots:

    void on_cbColor_clicked();

private:
    Ui::PointDisplayOptionsDialog *ui;
};

#endif // LINEDISPLAYOPTIONSDIALOG_H
