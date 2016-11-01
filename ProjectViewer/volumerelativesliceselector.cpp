#include "volumerelativesliceselector.h"
#include "ui_volumerelativesliceselector.h"

#include<QMessageBox>
#include<selectionpoint.h>

/* using reverse spinbox with internal negative values displayed positive for time.
 * This allows using cursor/arrow down for moving to greater times and thus deeper.
 * All calls to spinbox are modified accordingly, i.e. minimum becomes maximum and vice versa.
 * Spinbox values are allways reversed
*/

VolumeRelativeSliceSelector::VolumeRelativeSliceSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeRelativeSliceSelector)
{
    ui->setupUi(this);

    connect( ui->sbTime, SIGNAL(valueChanged(int)), this, SIGNAL(timeChanged(int)) );
    connect( this, SIGNAL(timeChanged(int)), this, SLOT(apply()) );
    connect(this, SIGNAL(volumeChanged(std::shared_ptr<Grid3D<float> >)), this, SLOT(apply()) );
}

VolumeRelativeSliceSelector::~VolumeRelativeSliceSelector()
{
    delete ui;
}

double VolumeRelativeSliceSelector::timeAt( int iline, int xline){

    if( !m_horizon ) return SelectionPoint::NO_TIME;

    double t=m_horizon->valueAt(iline, xline);
    if( t==m_horizon->NULL_VALUE) return SelectionPoint::NO_TIME;

    t+=time();
    t*=0.001;   // convert to seconds

    return t;
}

int VolumeRelativeSliceSelector::time(){

    return ui->sbTime->value();
}

std::pair<int, int>  VolumeRelativeSliceSelector::timeRange(){

    return std::pair<int,int>( ui->sbTime->minimum(), ui->sbTime->maximum());
}

int VolumeRelativeSliceSelector::timeStep(){

    return ui->sbTime->singleStep();
}

QString VolumeRelativeSliceSelector::description(){

    QString descr=QString("%1 @%2").arg(m_volumeName).arg(m_horizonName);
    if(time()>=0){
        descr+=QString(" +");
    }
    descr+=QString(" %1 ms").arg(time());

    return descr;
}

void VolumeRelativeSliceSelector::setTime(int t){

   ui->sbTime->setValue(t); // spinbox will take care of emitting change signal if necessary
}

void VolumeRelativeSliceSelector::setTimeRange(std::pair<int, int> r){

    //if( r.first==ui->sbTime->minimum() && r.second==ui->sbTime->maximum()) return;

    if( r==timeRange()) return;

    ui->sbTime->setMinimum(r.first);
    ui->sbTime->setMaximum(r.second);

    emit timeRangeChanged(timeRange());
}

void VolumeRelativeSliceSelector::setTimeStep(int s){

    if( s==timeStep()) return;

    ui->sbTime->setSingleStep(s);

    emit timeStepChanged(s);
}

void VolumeRelativeSliceSelector::setVolumeName(QString name){

    if( name==m_volumeName) return;

    m_volumeName=name;

    emit volumeNameChanged(name);
    emit descriptionChanged(description());
}

void VolumeRelativeSliceSelector::setHorizonName(QString name){

    if( name==m_horizonName) return;

    m_horizonName=name;

    emit horizonNameChanged(name);
    emit descriptionChanged(description());
}

void VolumeRelativeSliceSelector::setVolume(std::shared_ptr<Grid3D<float> > v){

    if( v==m_volume) return;

    m_volume=v;

    emit volumeChanged(v);
}

void VolumeRelativeSliceSelector::setGrid(std::shared_ptr<Grid2D<float> > g){

    if( m_grid==g) return;

    m_grid=g;

    emit gridChanged(g);
}


void VolumeRelativeSliceSelector::setHorizon(std::shared_ptr<Grid2D<float> > g){

    if( m_horizon==g) return;

    m_horizon=g;

    apply();

    emit horizonChanged(g);
}


void VolumeRelativeSliceSelector::setProject(std::shared_ptr<AVOProject> prj){

    if( prj==m_project) return;

    m_project=prj;

    ui->cbHorizon->clear();
    ui->cbHorizon->addItems( m_project->gridList(GridType::Horizon) );
}

void VolumeRelativeSliceSelector::apply(){

    if( !m_volume ){
        setGrid(std::shared_ptr<Grid2D<float>>() ); // set NULL grid if no volume
        return;
    }

    if( !m_horizon ){
        setGrid(std::shared_ptr<Grid2D<float>>() ); // set NULL grid if no horizon
        return;
    }

    double t=0.001 * time();  // convert msecs to seconds

    Grid3DBounds vbounds=m_volume->bounds();
    Grid2DBounds gbounds(vbounds.inline1(), vbounds.crossline1(), vbounds.inline2(), vbounds.crossline2());
    std::shared_ptr< Grid2D<float> > g( new Grid2D<float>(gbounds) );

    for( int iline=gbounds.i1(); iline<=gbounds.i2(); iline++){

        for( int xline=gbounds.j1(); xline<=gbounds.j2(); xline++ ){

            float horizonTime=(*m_horizon)(iline, xline);
            if( horizonTime==m_horizon->NULL_VALUE ) continue;

            horizonTime*=0.001;     // convert to seconds

            (*g)(iline, xline)=m_volume->value(iline, xline, horizonTime + t);
        }
    }

    setGrid(g);  // emits gridChanged

    emit descriptionChanged(description());
}

void VolumeRelativeSliceSelector::on_cbHorizon_currentIndexChanged(const QString& name)
{

    if( name.isNull() || !m_project){
        m_horizon.reset();
        m_horizonName.clear();
        return;
    }

    std::shared_ptr<Grid2D<float> > tmp=m_project->loadGrid(GridType::Horizon, name);
    if( !tmp ){

        QMessageBox::critical( this, tr("Load Horizon"), QString("Loading horizon \"%1\" failed!").arg(name));
        return;
    }

    setHorizon(tmp);
    setHorizonName(name);
}
