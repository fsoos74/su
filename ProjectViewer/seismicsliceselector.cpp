#include "seismicsliceselector.h"
#include "ui_seismicsliceselector.h"

#include<segyreader.h>

SeismicSliceSelector::SeismicSliceSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SeismicSliceSelector)
{
    ui->setupUi(this);
    m_timeValidator= new QIntValidator(this);
    ui->leTime->setValidator(m_timeValidator);
}

SeismicSliceSelector::~SeismicSliceSelector()
{
    delete ui;
}

int SeismicSliceSelector::time(){
    return ui->leTime->text().toInt();
}

std::pair<int,int> SeismicSliceSelector::timeRange(){
    return std::pair<int,int>(m_timeValidator->bottom(), m_timeValidator->top() );
}


void SeismicSliceSelector::setReader(std::shared_ptr<SeismicDatasetReader> r){

    if( r==m_reader) return;

    m_reader=r;

    apply();
}

void SeismicSliceSelector::setTime(int i){

    if( i==time() ) return;

    ui->leTime->setText(QString::number(i));

    apply();

    emit timeChanged(i);
}

void SeismicSliceSelector::setTimeRange(std::pair<int, int> r ){

    if( r.first==m_timeValidator->bottom() && r.second==m_timeValidator->top() ) return;

    m_timeValidator->setRange( r.first, r.second);

    emit timeRangeChanged(r);
}


QString SeismicSliceSelector::selectionDescription(){

    if( !m_reader ) return QString();

    QString name=m_reader->info().name();

    return QString("%1 - %2 ms").arg(name).arg(time());
}

void SeismicSliceSelector::on_leTime_returnPressed()
{
    apply();

    emit timeChanged(time());
}


void SeismicSliceSelector::keyPressEvent(QKeyEvent *ev){

    if( ev->key() == Qt::Key_Return){
        ev->accept(); // ate it
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}

void SeismicSliceSelector::apply(){

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
    emit descriptionChanged(selectionDescription());
}


