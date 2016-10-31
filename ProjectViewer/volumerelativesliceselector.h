#ifndef VOLUMERELATIVESLICESELECTOR_H
#define VOLUMERELATIVESLICESELECTOR_H

#include <QWidget>
#include<memory>
#include<grid2d.h>
#include<grid3d.h>
#include<avoproject.h>

namespace Ui {
class VolumeRelativeSliceSelector;
}

class VolumeRelativeSliceSelector : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeRelativeSliceSelector(QWidget *parent = 0);
    ~VolumeRelativeSliceSelector();

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
    void horizonNameChanged(QString);
    void descriptionChanged(QString);
    void volumeChanged( std::shared_ptr<Grid3D<float>>);
    void gridChanged( std::shared_ptr<Grid2D<float>>);
    void horizonChanged( std::shared_ptr<Grid2D<float>>);

public slots:

    void setTime(int);
    void setTimeRange(std::pair<int, int>);
    void setTimeStep(int);
    void setVolumeName(QString);
    void setVolume(std::shared_ptr<Grid3D<float>>);
    void setGrid( std::shared_ptr<Grid2D<float>>);
    void setHorizonName(QString);
    void setHorizon( std::shared_ptr<Grid2D<float>>);
    void setProject(std::shared_ptr<AVOProject> );

private slots:

    void apply();

    void on_cbHorizon_currentIndexChanged(const QString &arg1);

private:
    Ui::VolumeRelativeSliceSelector *ui;

    std::shared_ptr<AVOProject> m_project;

    QString m_volumeName;
    QString m_horizonName;

    std::shared_ptr<Grid3D<float>> m_volume;
    std::shared_ptr<Grid2D<float>> m_grid;
    std::shared_ptr<Grid2D<float>> m_horizon;
};

#endif // VOLUMERELATIVESLICESELECTOR_H
