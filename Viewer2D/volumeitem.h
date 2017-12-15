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

    double time()const{
        return m_time;
    }

    std::shared_ptr<Volume> volume()const{
        return m_volume;
    }

public slots:
    void setTime(double);
    void setVolume(std::shared_ptr<Volume>);

private slots:
    void updateGrid();

private:

    std::shared_ptr<Volume> m_volume;
    double m_time;

};

#endif // VOLUMEITEM_H
