#ifndef VOLUMEITEM_H
#define VOLUMEITEM_H

#include <viewitem.h>
#include "volume.h"
#include <colortable.h>
#include<memory>

namespace sliceviewer {


class VolumeItem : public ViewItem
{
    Q_OBJECT

public:

    enum class Style{ATTRIBUTE,SEISMIC};
    enum class Polarity{NORMAL, REVERSED};

    VolumeItem(QObject* parent=nullptr);
    std::shared_ptr<Volume> volume()const{
        return mVolume;
    }
    int opacity()const{
        return mOpacity;
    }
    ColorTable* colorTable()const{
        return mColorTable;
    }
    Style style()const{
        return mStyle;
    }
    Polarity polarity()const{
        return mPolarity;
    }
    qreal gain()const{
        return mGain;
    }

public slots:
    void setVolume(std::shared_ptr<Volume>);
    void setOpacity(int);
    void setStyle(VolumeItem::Style);
    void setPolarity(VolumeItem::Polarity);
    void setGain(qreal);

private:

    std::shared_ptr<Volume> mVolume;
    int mOpacity;
    Style mStyle;
    Polarity mPolarity;
    qreal mGain;
    ColorTable* mColorTable;
};

}

#endif // VOLUMEITEM_H
