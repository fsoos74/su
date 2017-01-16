#ifndef VOLUMENAVIGATIONDIALOG_H
#define VOLUMENAVIGATIONDIALOG_H

#include <QDialog>
#include <QVector3D>

namespace Ui {
class VolumeNavigationDialog;
}

class VolumeNavigationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeNavigationDialog(QWidget *parent = 0);
    ~VolumeNavigationDialog();

    QVector3D position();
    QVector3D rotation();
    QVector3D scale();

public slots:

    void setPosition(QVector3D);
    void setRotation(QVector3D);
    void setScale(QVector3D);

signals:

    void positionChanged(QVector3D);
    void rotationChanged(QVector3D);
    void scaleChanged(QVector3D);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);

private slots:

    void positionEntered();
    void rotationEntered();
    void scaleEntered();

private:
    Ui::VolumeNavigationDialog *ui;
};

#endif // VOLUMENAVIGATIONWIDGET_H
