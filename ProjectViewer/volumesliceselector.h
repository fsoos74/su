#ifndef VOLUMESLICESELECTOR_H
#define VOLUMESLICESELECTOR_H

#include <QWidget>
#include<memory>
#include<grid2d.h>
#include<grid3d.h>

namespace Ui {
class VolumeSliceSelector;
}

class VolumeSliceSelector : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeSliceSelector(QWidget *parent = 0);
    ~VolumeSliceSelector();

    int time();
    std::pair<int,int> timeRange();
    int timeStep();

    QString volumeName(){
        return m_volumeName;
    }

    QString description();

    std::shared_ptr<Grid3D<float>> volume()const{
        return m_volume;
    }

    std::shared_ptr<Grid2D<float>> grid()const{
        return m_grid;
    }

signals:

    void timeChanged(int);
    void timeRangeChanged(std::pair<int,int>);
    void timeStepChanged(int);
    void volumeNameChanged(QString);
    void descriptionChanged(QString);
    void volumeChanged( std::shared_ptr<Grid3D<float>>);
    void gridChanged( std::shared_ptr<Grid2D<float>>);

public slots:

    void setTime(int);
    void setTimeRange(std::pair<int, int>);
    void setTimeStep(int);
    void setVolumeName(QString);
    void setVolume(std::shared_ptr<Grid3D<float>>);
    void setGrid( std::shared_ptr<Grid2D<float>>);

private slots:

    void apply();

private:
    Ui::VolumeSliceSelector *ui;

    QString m_volumeName;

    std::shared_ptr<Grid3D<float>> m_volume;
    std::shared_ptr<Grid2D<float>> m_grid;
};

#endif // VOLUMESLICESELECTOR_H
