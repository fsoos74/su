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

    enum class LabelStyle{
        NO_LABEL,
        NAME_LABEL,
        UWI_LABEL,
        NAME_AND_UWI_LABEL
    };

    MarkerItem(QObject* parent=nullptr);
    // position x=iline, y=xline, z=z increasing upwards(-depth)
    QVector3D position()const{
        return mPosition;
    }
    QString markerName()const{
        return mMarkerName;
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
    LabelStyle labelStyle()const{
        return mLabelStyle;
    }
    int labelFontSize()const{
        return mLabelFontSize;
    }

public slots:
    void setPosition(QVector3D);
    void setMarkerName(QString);
    void setUwi(QString);
    void setColor(QColor);
    void setWidth(int);
    void setOpacity(int);
    void setLabelStyle(LabelStyle);
    void setLabelFontSize(int);

private:
    QVector3D mPosition;
    QString mMarkerName;
    QString mUwi;
    QColor mColor;
    int mWidth;
    int mOpacity;
    LabelStyle mLabelStyle;
    int mLabelFontSize;
};

}

#endif // MARKERITEM_H
