#ifndef SEISMICDATASELECTOR_H
#define SEISMICDATASELECTOR_H

#include <QDialog>
#include<QKeyEvent>
#include<memory>
#include <seismicdatasetreader.h>
#include <gather.h>


namespace Ui {
class SeismicDataSelector;
}

class SeismicDataSelector : public QDialog
{
    Q_OBJECT

public:

     static const int MAXIMUM_TRACES_PER_SCREEN=9999;

    enum Order{
        ORDER_NONE=0,
        ORDER_INLINE_ASCENDING,
        ORDER_INLINE_DESCENDING,
        ORDER_CROSSLINE_ASCENDING,
        ORDER_CROSSLINE_DESCENDING,
        ORDER_OFFSET_ASCENDING,
        ORDER_OFFSET_DESCENDING
    };

    explicit SeismicDataSelector(QWidget *parent = 0);
    ~SeismicDataSelector();

public slots:
    void setReader(std::shared_ptr<SeismicDatasetReader>);
    void setInlineRange( int min, int max);
    void setCrosslineRange( int min, int max);
    void setInlineCountRange( int min, int max);
    void setCrosslineCountRange( int min, int max);
    void setInline(int);
    void setCrossline(int);
    void setInlineCount( int count );
    void setCrosslineCount( int count );
    void setOrder(int, int, int);
    void apply();
    void providePoint(int iline, int xline);
    void provideRandomLine( QVector<QPoint> polyline);

signals:
    void gatherChanged(std::shared_ptr<seismic::Gather>);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);


private slots:
    void readGather();
    void changeOrder();

    void on_sbIlineCount_valueChanged(int arg1);

    void on_sbXlineCount_valueChanged(int arg1);

    void on_rbRandomLine_toggled(bool checked);

private:

    void indexToKey( int idx, QString& key, bool& ascending);

    Ui::SeismicDataSelector *ui;

    std::shared_ptr<SeismicDatasetReader>   m_reader;
    std::shared_ptr<seismic::Gather>        m_gather;
};

#endif // SEISMICDATASELECTOR_H
