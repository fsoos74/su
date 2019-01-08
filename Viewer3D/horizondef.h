#ifndef HHORIZONDEF_H
#define HHORIZONDEF_H


#include<memory>
#include<grid2d.h>
#include<QColor>


class HorizonDef{
public:
    enum class ColorStyle{ Fixed, Gradient, Volume};

    HorizonDef()=default;
    HorizonDef(std::shared_ptr<Grid2D<float>> horizon,
               ColorStyle cs, QColor color, qreal alpha, qreal colorGradient,
               QString volume, int delay);

    std::shared_ptr<Grid2D<float>> horizon()const{
        return m_horizon;
    }

    ColorStyle colorStyle()const{
        return m_colorStyle;
    }

    QColor color()const{
        return m_color;
    }

    qreal alpha()const{
        return m_alpha;
    }

    qreal colorGradient()const{
        return m_colorGradient;
    }

    QString volume()const{
        return m_volume;
    }

    int delay()const{
        return m_delay;
    }

    void setHorizon(std::shared_ptr<Grid2D<float>>);
    void setColorStyle(ColorStyle);
    void setColor(QColor);
    void setAlpha(qreal);
    void setColorGradient(qreal);
    void setVolume(QString);
    void setDelay(int);

    static QString toQString(HorizonDef::ColorStyle);
    static HorizonDef::ColorStyle toColorStyle(QString);

    std::shared_ptr<Grid2D<float>> m_horizon;
    ColorStyle m_colorStyle=ColorStyle::Fixed;
    QColor m_color=Qt::gray;
    qreal m_alpha;
    qreal m_colorGradient=0.;
    QString m_volume;
    int m_delay=0;
};


inline bool operator==(const HorizonDef& h1, const HorizonDef& h2){

    return (h1.horizon() == h2.horizon() ) &&
            (h1.colorStyle() == h2.colorStyle() ) &&
            (h1.volume() == h2.volume()) &&
            (h1.delay() == h2.delay()) &&
            (h1.color() == h2.color()) &&
            (h1.alpha() == h2.alpha()) &&
            (h1.colorGradient() == h2.colorGradient());
}


#endif // HORIZONDEF_H
