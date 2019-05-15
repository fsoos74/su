#ifndef MARKERITEM_H
#define MARKERITEM_H

#include "viewitem.h"
#include <QColor>
#include <QVector3D>

namespace sliceviewer {


// position will be set for all displayed wells by accessing marker name from db

class MarkerItem : public ViewItem{
    Q_OBJECT
public:
    MarkerItem(QObject* parent=nullptr);
    // position x=iline, y=xline, z=z increasing upwards(-depth)
    QVector3D position()const{
        return mPosition;
    }
    QString uwi()const{
        return mUwi;
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
    void setPosition(QVector3D);
    void setUwi(QString);
    void setColor(QColor);
    void setWidth(int);
    void setOpacity(int);

private:
    QVector3D mPosition;
    QString mUwi;
    QColor mColor;
    int mWidth;
    int mOpacity;
};

}

#endif // MARKERITEM_H
