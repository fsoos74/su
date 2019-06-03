#ifndef COMPASSWIDGET_H
#define COMPASSWIDGET_H

#include <QWidget>

namespace sliceviewer{


class CompassWidget : public QWidget
{
    Q_OBJECT
public:

    enum class Mode{
        NONE,
        NEEDLE,
        HORIZONTAL_DIRECTION,
        VERTICAL_DIRECTION
    };

    explicit CompassWidget(QWidget *parent = nullptr);

    Mode mode()const{
        return mMode;
    }

    float angle()const{
        return mAngle;
    }

signals:

public slots:
    void setMode(CompassWidget::Mode);
    void setAngle(float);

protected:
    virtual void paintEvent(QPaintEvent*)override;
    virtual void drawNeedle(QPainter&);

private:
    Mode mMode=Mode::NONE;
    float mAngle=0;
};

}

#endif // COMPASSWIDGET_H
