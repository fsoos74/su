#ifndef WELLITEM_H
#define WELLITEM_H

#include "viewitem.h"
#include <memory>
#include <wellpath.h>
#include <QColor>

namespace sliceviewer {


class WellItem : public ViewItem
{
    Q_OBJECT
public:
    enum class LabelStyle{
        NO_LABEL,
        NAME_LABEL,
        UWI_LABEL
    };

    explicit WellItem(QObject *parent = nullptr);
    QString uwi()const{
        return mUwi;
    }
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
    LabelStyle labelStyle()const{
        return mLabelStyle;
    }

    int labelFontSize()const{
        return mLabelFontSize;
    }

public slots:
    void setUwi(QString);
    void setWellPath(std::shared_ptr<WellPath>);
    void setColor(QColor);
    void setWidth(int);
    void setOpacity(int);
    void setLabelStyle(LabelStyle);
    void setLabelFontSize(int);

private:
    QString mUwi;
    std::shared_ptr<WellPath> mWellPath;
    QColor mColor;
    int mWidth;
    int mOpacity;
    LabelStyle mLabelStyle;
    int mLabelFontSize;
};

}

#endif // WELLITEM_H
