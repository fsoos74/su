#ifndef VOLUMEITEM_H
#define VOLUMEITEM_H

#include <viewitem.h>
#include "volume.h"
#include <colortable.h>
#include<memory>


class VolumeItem : public ViewItem
{
    Q_OBJECT

public:

    enum class Style{ATTRIBUTE=0,SEISMIC=1};

    VolumeItem(QObject* parent=nullptr);
    std::shared_ptr<Volume> volume(){
        return mVolume;
    }
    qreal opacity()const{
        return mOpacity;
    }
    ColorTable* colorTable()const{
        return mColorTable;
    }
    Style style()const{
        return mStyle;
    }

public slots:
    void setVolume(std::shared_ptr<Volume>);
    void setOpacity(qreal);
    void setStyle(VolumeItem::Style);

private:

    std::shared_ptr<Volume> mVolume;
    qreal mOpacity;
    Style mStyle;
    ColorTable* mColorTable;
};

#endif // VOLUMEITEM_H
