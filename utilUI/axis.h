#ifndef AXIS_H
#define AXIS_H

#include<vector>
#include<QObject>

class Axis : public QObject{

Q_OBJECT

public:

    enum class Type{ Linear, Logarithmic };

    struct Tick{
        enum class Type{ Main, Sub };
        Type type;
        qreal value;
        QString label;
    };

    static qreal NO_CURSOR;

    Axis( Type type=Type::Linear, qreal min=0, qreal max=0, QObject* parent=nullptr);

    QString name()const{
        return m_name;
    }

    QString unit()const{
        return m_unit;
    }

    bool isReversed()const{
        return m_reverse;
    }

    bool isAutomatic()const{
        return m_automatic;
    }

    qreal min()const{
        return m_min;
    }

    qreal max()const{
        return m_max;
    }

    Type type()const{
        return m_type;
    }

    qreal interval()const{
        return m_interval;
    }

    int subTickCount()const{
        return m_nsub;
    }

    bool labelSubTicks()const{
        return m_labelSubTicks;
    }

    qreal minSelection()const{
        return m_minSelection;
    }

    qreal maxSelection()const{
        return m_maxSelection;
    }

    qreal minView()const{
        return m_minView;
    }

    qreal maxView()const{
        return m_maxView;
    }

    qreal cursorPosition()const{
        return m_cursorPosition;
    }

    int labelPrecision()const{
        return m_labelPrecision;
    }

    qreal toScene(const qreal&)const;
    qreal toAxis(const qreal&)const;

    std::vector<Tick> computeTicks( qreal start, qreal stop)const;

public slots:

    void setName(QString);
    void setUnit(QString);
    void setRange(qreal, qreal);
    void setReversed(bool);
    void setType( Type);
    void setInterval( qreal );
    void setSubTickCount( int );
    void setLabelSubTicks( bool );
    void setAutomaticInterval( bool );
    void setSelection(qreal, qreal);
    void setCursorPosition( qreal );
    void setViewRange(qreal,qreal);
    void setLabelPrecision(int);

    void setViewPixelLength(int);
    void adjustRange();

signals:

    void nameChanged(QString);
    void unitChanged(QString);
    void reversedChanged(bool);
    void rangeChanged(qreal, qreal );
    void typeChanged( Type );
    void ticksChanged();
    void selectionChanged(qreal, qreal);
    void cursorPositionChanged( qreal );
    void viewRangeChanged(qreal,qreal);
    void labelPrecisionChanged(int);
    void viewPixelLengthChanged(int);

private:

    void computeAutomaticInterval();
    std::vector<Tick> computeTicksLinear( qreal start, qreal stop)const;
    std::vector<Tick> computeTicksLogarithmic( qreal start, qreal stop)const;

    QString m_name;
    QString m_unit;
    Type m_type;
    qreal m_min;
    qreal m_max;
    bool m_reverse=false;
    bool m_automatic=true;
    qreal m_interval=1;
    int m_nsub=9;
    bool m_labelSubTicks=false;
    qreal m_minView=0;                          // in axis coords
    qreal m_maxView=0;                          // in axis coords
    qreal m_minSelection=0;                     // in axis coords
    qreal m_maxSelection=0;                     // in axis coords
    qreal m_cursorPosition=NO_CURSOR;           // in axis coords
    int m_labelPrecision=6;                          // double conversion format for labels
    int m_viewPixelLength=100;
    Axis* m_buddy=nullptr;
};



#endif // AXIS_H
