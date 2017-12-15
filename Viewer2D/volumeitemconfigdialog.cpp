#include "volumeitemconfigdialog.h"

#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>

VolumeItemConfigDialog::VolumeItemConfigDialog(QWidget* parent):GridItemConfigDialog(parent)
{

    auto gb=new QGroupBox(tr("Volume"));
    auto hlayout=new QHBoxLayout;
    hlayout->addWidget(new QLabel("Slice Time (msec):"));
    m_sbTime=new QSpinBox;
    hlayout->addWidget(m_sbTime);
    gb->setLayout(hlayout);

    auto vlayout=dynamic_cast<QVBoxLayout*>(layout());
    Q_ASSERT(vlayout);
    vlayout->insertWidget(vlayout->count()-1, gb);

    connect( m_sbTime, SIGNAL(valueChanged(int)), this, SLOT(onSBTimeValueChanged(int)) );
}


int VolumeItemConfigDialog::time(){
    return m_sbTime->value();
}


void VolumeItemConfigDialog::setTimeRange(int min, int max){
    m_sbTime->setRange(min,max);
}

void VolumeItemConfigDialog::setTimeIncrement(int i){
    m_sbTime->setSingleStep(i);
}

void VolumeItemConfigDialog::setTime(int i){
    m_sbTime->setValue(i);
}

void VolumeItemConfigDialog::onSBTimeValueChanged(int msecs){

    emit timeChanged(0.001*msecs);
}
