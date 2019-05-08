#ifndef V2D_VOLUMEITEM_H
#define V2D_VOLUMEITEM_H

#include <griditem.h>
#include <QObject>
#include <memory>
#include <volume.h>

class V2DVolumeItem : public GridItem
{
    Q_OBJECT
public:
    V2DVolumeItem(AVOProject* project=nullptr, QGraphicsItem* parentGraphicsItemItem=nullptr, QObject* parentQObject=nullptr );

    int sliceValue()const{
        return m_sliceValue;
    }

    std::shared_ptr<Volume> volume()const{
        return m_volume;
    }

public slots:
    void setSliceValue(int);
    void setVolume(std::shared_ptr<Volume>);
    void setZValueWrapper(int);

private slots:
    void updateGrid();

private:

    std::shared_ptr<Volume> m_volume;
    int m_sliceValue;
};

#endif //V2D_VOLUMEITEM_H
