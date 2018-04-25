#ifndef COLORBAR_H
#define COLORBAR_H

#include <QObject>
#include <QRgb>
#include <QVector>
#include <QMap>
#include <functional>


class ColorBar : public QObject
{
    Q_OBJECT

public:

    enum class MapMode{Nearest, Interpolate};

    explicit ColorBar( int size=256, QObject *parent = nullptr);

    int size()const{
        return m_entries.size();
    }

    QRgb nullColor()const{
        return m_nullColor;
    }

    double nullValue()const{
        return m_nullValue;
    }

    MapMode mapMode()const{
        return m_mapMode;
    }

    bool isLocked()const{
        return m_locked;
    }

    std::pair<double,QRgb> get(int i)const{
        return (i>=0 && i<m_entries.size()) ? m_entries[i] : std::make_pair(m_nullValue, m_nullColor);
    }

    QRgb color(int i)const{
        return (i>=0 && i<m_entries.size()) ? m_entries[i].second : m_nullColor;
    }

    double value(int i)const{
        return (i>=0 && i<m_entries.size()) ? m_entries[i].first : m_nullValue;
    }

    QRgb map(double value)const;

    static QVector<double> linearValues(double first, double last, int n);
    static QVector<double> powerValues(double first, double last, double power, int n);

signals:
    void changed();
    void lockChanged(bool);

public slots:
    void setMapMode(MapMode);
    void setLocked(bool);
    void set(int, double, QRgb);
    void set(int, std::pair<double,QRgb>);
    void set(QVector<double>, QVector<QRgb>);
    void setColor(int,QRgb);
    void setColors(QVector<QRgb>);
    void setValue(int,double);
    void setValues(QVector<double>);
    void setNullColor(QRgb);
    void setNullValue(double);

private:
    void updateMapFunc();
    void updateLookup();
    QRgb mapInterpolated(double)const;
    QRgb mapNearest(double)const;

    QVector<std::pair<double,QRgb> > m_entries;
    QRgb m_nullColor=qRgba(0,0,0,0);
    double m_nullValue=std::numeric_limits<double>::lowest();
    bool m_locked=false;
    MapMode m_mapMode=MapMode::Interpolate;

    std::function<QRgb(double)> m_mapFunc;
    QMap<double, int> m_lookup;
};

#endif // COLORBAR_H
