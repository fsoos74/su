#include "editslicesdialog.h"
#include "ui_editslicesdialog.h"

EditSlicesDialog::EditSlicesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSlicesDialog)
{
    ui->setupUi(this);

    ui->cbType->addItem(toQString(SliceType::INLINE));
    ui->cbType->addItem(toQString(SliceType::CROSSLINE));
    ui->cbType->addItem(toQString(SliceType::SAMPLE));

    connect( ui->cbName, SIGNAL(currentIndexChanged(int)), this, SLOT(sliceToControls(int)) );
}

EditSlicesDialog::~EditSlicesDialog()
{
    delete ui;
}
/*
void EditSlicesDialog::setViewer( VolumeViewer* viewer){

    m_viewer=viewer;

    ui->cbName->clear();

    if( !viewer ) return;
}

void EditSlicesDialog::addSlice(SliceDef s){

    m_slices.append(s);
    QString name=QString("%1 %2").arg(toQString(s.type)).arg(s.value);
    ui->cbName->addItem(name);
    emit sliceAdded(s);
}


void EditSlicesDialog::removeSlice(SliceDef s){

    int idx=m_slices.indexOf(s);
    if( idx<0 ) return;

    m_slices.remove(idx);
    ui->cbName->removeItem(idx);    // slices and names in sync
    emit sliceRemoved(s);
}

*/
void EditSlicesDialog::sliceToControls(int i){
/*
    if( !m_viewer) return;

    SliceDef s=m_viewer->slices().at(i);

    ui->cbName->setCurrentIndex(i);

    int idx=ui->cbType->findText(toQString(s.type));
    if( idx<0 ) return;

    ui->cbType->setCurrentIndex(idx);

    switch( s.type ){

    case SliceType::INLINE: ui->sbValue->setRange(m_bounds.inline1(), m_bounds.inline2()); break;
    case SliceType::CROSSLINE: ui->sbValue->setRange(m_bounds.crossline1(), m_bounds.crossline2()); break;
    case SliceType::SAMPLE: ui->sbValue->setRange(0, m_bounds.sampleCount()); break;
    }

    ui->sbValue->setValue(s.value);
    */
}

