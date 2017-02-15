#ifndef COLORTABLE_H
#define COLORTABLE_H

#include <QObject>
#include <QVector>
#include <QRgb>
#include <QColor>


extern const int COLORTABLE_SIZE;




class ColorTable : public QObject
{
    Q_OBJECT
public:

    typedef QRgb color_type;

    explicit ColorTable(QObject *parent = 0);

    ColorTable( QObject* parent, const QVector<color_type>& cols, const  std::pair<double,double>& range );

    const QVector<color_type>& colors()const{
        return m_colors;
    }

     std::pair<double,double> range()const{
         return m_range;
     }

     double power()const{
         return m_power;
     }

     bool isLocked()const{
         return m_lock;
     }

     color_type map( const double val )const;

     static QVector<color_type> defaultColors();
     static QVector<color_type> grayscale();
     static void blend( QVector<QRgb>& cols, int first, int last);


signals:

    void colorsChanged();
    void rangeChanged( std::pair<double,double>  );
    void powerChanged( double );
    bool lockedChanged(bool);

public slots:

    void setColors( const QVector<QRgb>& );
    void setRange( const  std::pair<double,double> & );
    void setRange(double, double);
    void setPower(double);
    void setLocked(bool);

private:

    QVector<color_type> m_colors;
    std::pair<double, double> m_range;
    double m_power=1;
    bool m_lock=false;
};

#endif // COLORTABLE_H
