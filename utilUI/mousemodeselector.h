#ifndef MOUSEMODESELECTOR_H
#define MOUSEMODESELECTOR_H

#include <QWidget>

#include "mousemode.h"

namespace Ui {
class MouseModeSelector;
}

class MouseModeSelector : public QWidget
{
    Q_OBJECT

public:
    explicit MouseModeSelector(QWidget *parent = 0);
    ~MouseModeSelector();

    MouseMode mode();

signals:
    void modeChanged(MouseMode);

public slots:
    void setMode(MouseMode);

private slots:
    void onButtonBox();

private:
    Ui::MouseModeSelector *ui;
};

#endif // MOUSEMODESELECTOR_H
