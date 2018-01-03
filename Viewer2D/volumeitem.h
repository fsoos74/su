#ifndef VOLUMEITEM_H
#define VOLUMEITEM_H

#include <griditem.h>
#include <QObject>
#include <memory>
#include <volume.h>

class VolumeItem : public GridItem
{
    Q_OBJECT
public:
    VolumeItem(AVOProject* project=nullptr, QGraphicsItem* parentGraphicsItemItem=nullptr, QObject* parentQObject=nullptr );

    double refDepth()const{
        return m_refDepth;
    }

    std::shared_ptr<Volume> volume()const{
        return m_volume;
    }

public slots:
    void setRefDepth(double);
    void setVolume(std::shared_ptr<Volume>);

private slots:
    void updateGrid();

private:

    std::shared_ptr<Volume> m_volume;
    double m_refDepth;

};

#endif // VOLUMEITEM_H
