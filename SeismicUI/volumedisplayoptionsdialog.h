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

    int opacity();

public slots:

    void setEditColorTableAction(QAction* action);
    void setVolumeDisplayRangeAction(QAction* action);
    void setOpacity( int );

signals:

    void opacityChanged( int );

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);


private slots:

    void on_pbColorTable_clicked();

    void on_pbDisplayRange_clicked();

private:
    Ui::VolumeDisplayOptionsDialog *ui;

    QAction* m_editColorTableAction=nullptr;
    QAction* m_volumeDisplayRangeAction=nullptr;
};

#endif // VOLUMEDISPLAYOPTIONSDIALOG_H
