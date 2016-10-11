#include "sliceselector.h"
#include "ui_sliceselector.h"

#include<segyreader.h>

SliceSelector::SliceSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SliceSelector)
{
    ui->setupUi(this);
}

SliceSelector::~SliceSelector()
{
    delete ui;
}

int SliceSelector::time(){
    return ui->sbTime->value();
}

std::pair<int,int> SliceSelector::timeRange(){
    return std::pair<int,int>(ui->sbTime->minimum(), ui->sbTime->maximum());
}

int SliceSelector::timeIncrement(){
    return ui->sbTime->value();
}

void SliceSelector::setReader(std::shared_ptr<SeismicDatasetReader> r){

    if( r==m_reader) return;

    m_reader=r;

    apply();
}

void SliceSelector::setTime(int i){
    // this handles all signal emitting
    ui->sbTime->setValue(i);
}

void SliceSelector::setTimeRange(std::pair<int, int> r ){

    if( r.first==ui->sbTime->minimum() && r.second==ui->sbTime->maximum()) return;

    ui->sbTime->setMinimum(r.first);
    ui->sbTime->setMaximum(r.second);

    emit timeRangeChanged(r);
}

void SliceSelector::setTimeIncrement(int i){

    if(i==ui->sbTime->singleStep()) return;

    ui->sbTime->setSingleStep(i);

    emit timeIncrementChanged(i);
}

void SliceSelector::on_sbTime_valueChanged(int i)
{

    apply();

    emit timeChanged(i);
}

void SliceSelector::apply(){

    if( !m_reader ){
        m_slice.reset();
        emit sliceChanged(m_slice);
    }

    int sliceTime=time();


    Grid2DBounds bounds(m_reader->minInline(), m_reader->minCrossline(),
                        m_reader->maxInline(), m_reader->maxCrossline());
    std::shared_ptr<Grid2D<float>> slice(new Grid2D<float>(bounds));

    std::shared_ptr<seismic::SEGYReader> sreader=m_reader->segyReader();
    sreader->seek_trace(0);

    while( sreader->current_trace()<sreader->trace_count()){

        seismic::Trace trace=sreader->read_trace();
        const seismic::Header& header=trace.header();
        int iline=header.at("iline").intValue();
        int xline=header.at("xline").intValue();

        qreal t=0.001*sliceTime;  // slicetime in millisecs

        // interpolate between nearest samples, should add this to trace
        qreal x=(t - trace.ft() )/trace.dt();
        size_t i=std::truncf(x);
        x-=i;
        if( x>=0 || i<trace.samples().size()){
            (*slice)(iline, xline)=( 1.-x) * trace.samples()[i] + x*trace.samples()[i+1];
        }

    }

    m_slice=slice;

    emit(sliceChanged(slice));
}
