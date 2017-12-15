#ifndef TRACKRULERVIEW_H
#define TRACKRULERVIEW_H

#include <QPainterPath>
#include <log.h>
#include <memory>
#include<ruleraxisview.h>

class TrackRulerView : public RulerAxisView
{
Q_OBJECT

public:
    TrackRulerView(QWidget* parent);

    std::shared_ptr<Log> log(){
        return m_log;
    }

public slots:
    void setLog( std::shared_ptr<Log> );
    void setHighlighted(QVector<qreal>);

protected:
    void refreshScene()override;

private:
    QPainterPath buildPath(const Log&);

    std::shared_ptr<Log> m_log;
    QVector<qreal> m_highlighted;
};

#endif // TRACKRULERVIEW_H
