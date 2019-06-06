#ifndef HORIZONITEM_H
#define HORIZONITEM_H

#include "viewitem.h"
#include <grid2d.h>
#include <memory>
#include <QColor>

namespace sliceviewer {


class HorizonItem : public ViewItem
{
    Q_OBJECT
public:
    explicit HorizonItem(QObject *parent = nullptr);
    std::shared_ptr<Grid2D<float>> horizon()const{
        return mHorizon;
    }
    QColor color()const{
        return mColor;
    }
    int width()const{
        return mWidth;
    }
    int opacity()const{
        return mOpacity;
    }

public slots:
    void setHorizon(std::shared_ptr<Grid2D<float>>);
    void setColor(QColor);
    void setWidth(int);
    void setOpacity(int);

private:
    std::shared_ptr<Grid2D<float>> mHorizon;
    QColor mColor=Qt::red;
    int mWidth=2;
    int mOpacity=100;
};

}

#endif // HORIZONITEM_H
