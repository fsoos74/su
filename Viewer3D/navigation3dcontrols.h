#ifndef NAVIGATION3DCONTROLS_H
#define NAVIGATION3DCONTROLS_H

#include <QWidget>

namespace Ui {
class Navigation3DControls;
}

class Navigation3DControls : public QWidget
{
    Q_OBJECT

public:
    explicit Navigation3DControls(QWidget *parent = 0);
    ~Navigation3DControls();

signals:

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void moveBack();
    void moveFront();

    void rotateXNeg();
    void rotateXPos();
    void rotateYNeg();
    void rotateYPos();
    void rotateZNeg();
    void rotateZPos();

    void scaleXNeg();
    void scaleXPos();
    void scaleYNeg();
    void scaleYPos();
    void scaleZNeg();
    void scaleZPos();

private:
    Ui::Navigation3DControls *ui;
};

#endif // NAVIGATION3DCONTROLS_H
