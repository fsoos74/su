#ifndef SLICESELECTOR_H
#define SLICESELECTOR_H

#include <QWidget>
#include <memory>
#include <grid2d.h>
#include <seismicdatasetreader.h>
#include <QIntValidator>
#include <QKeyEvent>


namespace Ui {
class SeismicSliceSelector;
}

// time in milliseconds
class SeismicSliceSelector : public QWidget
{
    Q_OBJECT

public:
    explicit SeismicSliceSelector(QWidget *parent = 0);
    ~SeismicSliceSelector();

    std::shared_ptr<Grid2D<float>> slice()const{
        return m_slice;
    }

    int time();

    std::pair<int,int> timeRange();

    QString selectionDescription();

public slots:
    void setReader(std::shared_ptr<SeismicDatasetReader>);
    void setTime(int);
    void setTimeRange( std::pair<int,int>);

    void apply();

signals:

    void timeChanged(int);
    void timeRangeChanged(std::pair<int,int>);
    void sliceChanged(std::shared_ptr<Grid2D<float>>);
    void descriptionChanged(QString);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);


private slots:

    void on_leTime_returnPressed();

private:
    Ui::SeismicSliceSelector *ui;

    QIntValidator* m_timeValidator=nullptr;
    std::shared_ptr<SeismicDatasetReader>   m_reader;
    std::shared_ptr<Grid2D<float>> m_slice;
};

#endif // SLICESELECTOR_H
