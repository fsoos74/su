#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include<QScrollArea>
#include<QLabel>
#include<QVector>
#include<QRgb>
#include<memory>

#include<grid2d.h>

class GridView;

class Ruler : public QWidget{

public:

    enum Orientation{ VERTICAL_RULER, HORIZONTAL_RULER };

    const int TICK_MARK_SIZE=5;
    const int TICK_LABEL_DX=100;
    const int TICK_LABEL_DY=100;

    Ruler( GridView* parent, Orientation orient );
    const QVector<int> currentTicks() const;

protected:
    void paintEvent( QPaintEvent*);

private:

    void computeCurrentTicks(int first_val, int end_val, int incr);

    void drawVertical( QPainter& painter );
    void drawHorizontal( QPainter& painter );

    GridView* m_view;
    Orientation m_orientation;
     QVector<int> m_currentTicks;
};


class GridView : public QScrollArea
{
friend class Ruler;
Q_OBJECT

public:

    const int RULER_WIDTH=50;
    const int RULER_HEIGHT=30;

    const int MAX_COLORS=256;
    using ColorTable=QVector<QRgb>;


    GridView(QWidget* parent=nullptr);

    ~GridView();

    QLabel* label()const{ return m_label;}

    std::shared_ptr<Grid2D> grid()const{
        return m_grid;                   //!!!!
    }

    std::pair<double, double> colorMapping()const{
        return m_map;
    }

    const ColorTable& colorTable()const{
        return m_colorTable;
    }

    double scale()const{
        return m_scale;
    }

signals:

    void gridChanged( std::shared_ptr<Grid2D>);
    void colorMappingChanged( std::pair<double,double>);
    void colorTableChanged( const ColorTable&);
    void scaleChanged(double);

public slots:

    void setGrid( std::shared_ptr<Grid2D> );
    void setColorMapping( const std::pair<double,double>& );
    void setColorTable( const ColorTable&);
    void setScale( double );

protected:

    void resizeEvent( QResizeEvent*);

private:

    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updatePixmap( bool keepSize=false );
    void updateLayout();
    QImage grid2image();


    QLabel* m_label=nullptr;
    Ruler* m_leftRuler=nullptr;
    Ruler* m_topRuler=nullptr;
    std::shared_ptr<Grid2D> m_grid;
    std::pair<double,double> m_map;             // values corresponding minimum and maximum colors
    ColorTable m_colorTable;
    double m_scale=1;
};



#endif // GRIDVIEW_H
