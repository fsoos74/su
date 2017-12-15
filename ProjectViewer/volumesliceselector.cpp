#include "volumesliceselector.h"
#include "ui_volumesliceselector.h"


/* using reverse spinbox with internal negative values displayed positive for time.
 * This allows using cursor/arrow down for moving to greater times and thus deeper.
 * All calls to spinbox are modified accordingly, i.e. minimum becomes maximum and vice versa.
 * Spinbox values are allways reversed
*/

VolumeSliceSelector::VolumeSliceSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeSliceSelector)
{
    ui->setupUi(this);

    connect( ui->sbTime, SIGNAL(valueChanged(int)), this, SIGNAL(timeChanged(int)) );
    connect( this, SIGNAL(timeChanged(int)), this, SLOT(apply()) );
    connect(this, SIGNAL(volumeChanged(std::shared_ptr<Grid3D<float> >)), this, SLOT(apply()) );
}

VolumeSliceSelector::~VolumeSliceSelector()
{
    delete ui;
}


int VolumeSliceSelector::time(){

    return ui->sbTime->value();
}

std::pair<int, int>  VolumeSliceSelector::timeRange(){

    return std::pair<int,int>( ui->sbTime->minimum(), ui->sbTime->maximum());
}

int VolumeSliceSelector::timeStep(){

    return ui->sbTime->singleStep();
}

QString VolumeSliceSelector::description(){

    return QString("%1 - %2 ms").arg(m_volumeName).arg(time());
}

void VolumeSliceSelector::setTime(int t){

   ui->sbTime->setValue(t); // spinbox will take care of emitting change signal if necessary
}

void VolumeSliceSelector::setTimeRange(std::pair<int, int> r){

    //if( r.first==ui->sbTime->minimum() && r.second==ui->sbTime->maximum()) return;

    if( r==timeRange()) return;

    ui->sbTime->setMinimum(r.first);
    ui->sbTime->setMaximum(r.second);

    emit timeRangeChanged(timeRange());
}

void VolumeSliceSelector::setTimeStep(int s){

    if( s==timeStep()) return;

    ui->sbTime->setSingleStep(s);

    emit timeStepChanged(s);
}

void VolumeSliceSelector::setVolumeName(QString name){

    if( name==m_volumeName) return;

    m_volumeName=name;

    emit volumeNameChanged(name);
    emit descriptionChanged(description());
}

void VolumeSliceSelector::setVolume(std::shared_ptr<Grid3D<float> > v){

    if( v==m_volume) return;

    m_volume=v;

    emit volumeChanged(v);
}

void VolumeSliceSelector::setGrid(std::shared_ptr<Grid2D<float> > g){

    if( m_grid==g) return;

    m_grid=g;

    emit gridChanged(g);
}


void VolumeSliceSelector::apply(){

    if( !m_volume ){
        setGrid(std::shared_ptr<Grid2D<float>>() ); // set NULL grid if no volume
        return;
    }

    double t=0.001 * time();  // convert msecs to seconds

    Grid3DBounds vbounds=m_volume->bounds();
    Grid2DBounds gbounds(vbounds.i1(), vbounds.j1(), vbounds.i2(), vbounds.j2());
    std::shared_ptr< Grid2D<float> > g( new Grid2D<float>(gbounds) );

    for( int iline=gbounds.i1(); iline<=gbounds.i2(); iline++){

        for( int xline=gbounds.j1(); xline<=gbounds.j2(); xline++ ){

            (*g)(iline, xline)=m_volume->value(iline, xline, t);
        }
    }

    setGrid(g);  // emits gridChanged

    emit descriptionChanged(description());
}
