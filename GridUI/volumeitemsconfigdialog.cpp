#include "volumeitemsconfigdialog.h"

#include<QGridLayout>
#include <QLabel>
#include<QDialogButtonBox>
#include<QIntValidator>
#include<QDoubleValidator>

#include <colortabledialog.h>
#include <histogramcreator.h>
#include <histogramrangeselectiondialog.h>


VolumeItemsConfigDialog::VolumeItemsConfigDialog(QWidget* parent)
    : QDialog(parent){

    QGridLayout* lo=new QGridLayout;
    setLayout(lo);

    lo->addWidget(new QLabel("Volume"), 0, 0);
    lo->addWidget(new QLabel("Opacity"), 0, 1);

    QPushButton* pbClose=new QPushButton("Close", this);
    connect( pbClose, SIGNAL(clicked(bool)), this, SLOT(close()) );
    QHBoxLayout* hlo=new QHBoxLayout;
    hlo->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    hlo->addWidget(pbClose);
    hlo->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    lo->addLayout(hlo, 99, 0, 1, 4, Qt::AlignRight);
}

void VolumeItemsConfigDialog::addVolumeItem(QString name, qreal alpha, ColorTable *ct, const Histogram& hist){

    QGridLayout* lo=dynamic_cast<QGridLayout*>(layout());
    if( !lo ) return;

    auto label=new QLabel(name, this);

    auto sbalpha=new QDoubleSpinBox(this);
    sbalpha->setRange(0,1);
    sbalpha->setDecimals(2);
    sbalpha->setValue(alpha);
    connect( sbalpha, SIGNAL(valueChanged(double)), this, SLOT(onSBAlphaValueChanged(double)));

    auto pbRange=new QPushButton("", this);
    pbRange->setIcon(QIcon(":/icons/images/select-range-icon-32x32.png"));
    connect(pbRange, SIGNAL(clicked(bool)), this, SLOT(onPBRangeClicked()));

    auto pbColors=new QPushButton("", this);
    pbColors->setIcon(QIcon(":/icons/images/colortable-icon-32x32.png"));
    connect(pbColors, SIGNAL(clicked(bool)), this, SLOT(onPBColorsClicked()));

    int row=m_labels.size()+1;
    int col=0;
    lo->addWidget( label, row, col++);
    lo->addWidget( sbalpha, row, col++);
    lo->addWidget( pbRange, row, col++);
    lo->addWidget( pbColors, row, col++);

    m_labels.push_back(label);
    m_sbAlpha.push_back(sbalpha);
    m_pbRange.push_back(pbRange);
    m_pbColors.push_back(pbColors);
    m_colorTable.push_back(ct);
    m_histograms.push_back(hist);
}


void VolumeItemsConfigDialog::onSBAlphaValueChanged(double v){
    auto w = sender();
    auto sb = dynamic_cast<QDoubleSpinBox*>(w);
    if(!sb) return;
    auto idx = m_sbAlpha.indexOf(sb);
    auto name=m_labels[idx]->text();
    emit alphaChanged(name, v);
}


void VolumeItemsConfigDialog::onPBColorsClicked(){
    auto w = sender();
    auto pb = dynamic_cast<QPushButton*>(w);
    if(!pb) return;
    auto idx = m_pbColors.indexOf(pb);
    auto name=m_labels[idx]->text();

    auto ct=m_colorTable[idx];
    if( !ct ) return;

    QVector<QRgb> oldColors=ct->colors();

    ColorTableDialog colorTableDialog( oldColors);
    colorTableDialog.setWindowTitle(QString("Colortable - %1").arg(name));

    if( colorTableDialog.exec()==QDialog::Accepted ){
        ct->setColors( colorTableDialog.colors());
    }else{
        ct->setColors( oldColors );
    }
}


void VolumeItemsConfigDialog::onPBRangeClicked(){
    auto w = sender();
    auto pb = dynamic_cast<QPushButton*>(w);
    if(!pb) return;
    auto idx = m_pbRange.indexOf(pb);
    auto name=m_labels[idx]->text();

    auto ct=m_colorTable[idx];
    if( !ct ) return;

    HistogramRangeSelectionDialog displayRangeDialog;
    displayRangeDialog.setWindowTitle(QString("Display Range - %1").arg(name) );
    displayRangeDialog.setRange(ct->range());
    displayRangeDialog.setColorTable(ct);       // all connections with colortable are made by dialog
    displayRangeDialog.setHistogram(m_histograms[idx]);

    displayRangeDialog.exec();
}



