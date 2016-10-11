#ifndef SLICESELECTOR_H
#define SLICESELECTOR_H

#include <QWidget>
#include <memory>
#include <grid2d.h>
#include <seismicdatasetreader.h>


namespace Ui {
class SliceSelector;
}

// time in milliseconds
class SliceSelector : public QWidget
{
    Q_OBJECT

public:
    explicit SliceSelector(QWidget *parent = 0);
    ~SliceSelector();

    std::shared_ptr<Grid2D<float>> slice()const{
        return m_slice;
    }

    int time();
    std::pair<int,int> timeRange();
    int timeIncrement();

public slots:
    void setReader(std::shared_ptr<SeismicDatasetReader>);
    void setTime(int);
    void setTimeRange( std::pair<int,int>);
    void setTimeIncrement(int);

signals:

    void timeChanged(int);
    void timeRangeChanged(std::pair<int,int>);
    void timeIncrementChanged(int);
    void sliceChanged(std::shared_ptr<Grid2D<float>>);

private slots:
    void on_sbTime_valueChanged(int arg1);

private:
    Ui::SliceSelector *ui;

    void apply();

    std::shared_ptr<SeismicDatasetReader>   m_reader;
    std::shared_ptr<Grid2D<float>> m_slice;
};

#endif // SLICESELECTOR_H
