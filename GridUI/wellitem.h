#ifndef WELLITEM_H
#define WELLITEM_H

#include "viewitem.h"
#include <memory>
#include <wellpath.h>
#include <QColor>

class WellItem : public ViewItem
{
    Q_OBJECT
public:
    explicit WellItem(QObject *parent = nullptr);
    std::shared_ptr<WellPath> wellPath()const{
        return mWellPath;
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
    void setWellPath(std::shared_ptr<WellPath>);
    void setColor(QColor);
    void setWidth(int);
    void setOpacity(int);

private:
    std::shared_ptr<WellPath> mWellPath;
    QColor mColor;
    int mWidth;
    int mOpacity;
};

#endif // WELLITEM_H
