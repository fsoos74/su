#ifndef VOLUMEDISPLAYOPTIONSDIALOG_H
#define VOLUMEDISPLAYOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class VolumeDisplayOptionsDialog;
}

class VolumeDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeDisplayOptionsDialog(QWidget *parent = 0);
    ~VolumeDisplayOptionsDialog();

    std::pair<double, double> range();
    int opacity();

public slots:

    void setRange(std::pair<double,double>);
    void setOpacity( int );
    void setColors( QVector<QRgb>);

signals:

    void rangeChanged( std::pair<double, double>);
    void opacityChanged( int );
    void colorsChanged( QVector<QRgb>);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);


private slots:

    void applyRange();

    void on_pbColorTable_clicked();

private:
    Ui::VolumeDisplayOptionsDialog *ui;

    QVector<QRgb> m_colors;
};

#endif // VOLUMEDISPLAYOPTIONSDIALOG_H
