#ifndef GATHERRULER_H
#define GATHERRULER_H

#include <QWidget>
#include <QVector>
#include <QStringList>


class GatherView;

class GatherRuler : public QWidget
{
    Q_OBJECT

public:

    enum Orientation{ VERTICAL_RULER, HORIZONTAL_RULER };

    const int TICK_MARK_SIZE=5;
    const int TICK_LABEL_DX=60;
    const int TICK_LABEL_DY=40;


    class Tick{

    public:

        Tick()=default;

        Tick( const qreal& c, const QString& str):m_coord(c), m_annotations(str){

        }

        Tick( const qreal& c, const QStringList& strList):m_coord(c), m_annotations(strList){

        }

        qreal coord()const{
            return m_coord;
        }

        const QStringList& annotations()const{
            return m_annotations;
        }

    private:

        qreal m_coord;
        QStringList m_annotations;
    };

    GatherRuler(GatherView *parent, Orientation orient);

    Orientation orientation()const{
        return m_orientation;
    }

    const QStringList& axxisLabels()const{
        return m_axxisLabels;
    }

    qreal currentPos(){
        return m_currentPos;
    }

    virtual QVector< Tick > computeTicks( int pixFrom, int pixTo )const;

signals:

    void axxisLabelsChanged( const QStringList& );
    void clicked(qreal);

public slots:

    void setAxxisLabels(const QStringList&);
    void setCurrentPos(qreal);

protected:

    void paintEvent( QPaintEvent*);

    void drawVertical( QPainter& painter );
    void drawHorizontal( QPainter& painter );
    void drawHorizontalIndicator( QPainter&, QPoint );
    void drawVerticalIndicator( QPainter&, QPoint );

private:

    int tickIncrement(int size_pix, int size_data, int min_distance)const;

    GatherView* m_view;
    Orientation m_orientation;
    QStringList m_axxisLabels;
    qreal m_currentPos;
};

#endif // GATHERRULER_H
