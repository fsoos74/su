#ifndef HISTOGRAMCREATOR_H
#define HISTOGRAMCREATOR_H

#include <QObject>
#include <histogram.h>
#include <QProgressDialog>
#include <QApplication>

class HistogramCreator : public QObject
{
    Q_OBJECT
public:
    explicit HistogramCreator(QObject *parent = 0);

    template<typename ITERATOR, typename VALUE>
    Histogram createHistogram( ITERATOR begin, ITERATOR end, VALUE NULL_VALUE, VALUE min, VALUE max, size_t STEPS );

    template<typename ITERATOR, typename VALUE>
    Histogram createHistogram( ITERATOR begin, ITERATOR end, VALUE NULL_VALUE, size_t STEPS);

    // this will show a progressDialog
    template<typename ITERATOR, typename VALUE>
    Histogram createHistogram( QString progressTitle, ITERATOR begin, ITERATOR end, VALUE NULL_VALUE, size_t STEPS);

signals:
    void percentDone(int);

public slots:
};


// this does 50 % - 100 %
template<typename ITERATOR, typename VALUE>
Histogram HistogramCreator::createHistogram( ITERATOR begin, ITERATOR end, VALUE NULL_VALUE, VALUE min, VALUE max, size_t STEPS){

    if( min>=max ) return Histogram();

    Histogram histogram( min, (max-min)/STEPS, STEPS);

    auto n_total=std::distance(begin, end);
    size_t chunk_size=n_total/100;
    size_t n_processed=0;
    auto it=begin;

    while( n_processed<n_total){

            size_t n_to_process = std::min( chunk_size, n_total-n_processed);

            for( size_t i=0; i<n_to_process; i++){
                auto value=*it++;
                if( value!=NULL_VALUE) histogram.addValue( value);
            }

            n_processed+=n_to_process;

            size_t perc=50 + 100*n_processed/n_total/2;
            emit percentDone(perc);
            qApp->processEvents();
    }

    return histogram;
}


// thisdoes 0% - 50 %
template<typename ITERATOR, typename VALUE>
Histogram HistogramCreator::createHistogram( ITERATOR begin, ITERATOR end, VALUE NULL_VALUE, size_t STEPS){

    auto min=std::numeric_limits<VALUE>::max();
    auto max=std::numeric_limits<VALUE>::lowest();


    auto n_total=std::distance(begin, end);
    size_t chunk_size=n_total/100;
    size_t n_processed=0;
    auto it=begin;

    while( n_processed<n_total){

        size_t n_to_process = std::min( chunk_size, n_total-n_processed);

        for( size_t i=0; i<n_to_process; i++){
            auto value=*it++;
            if( value==NULL_VALUE) continue;
            if( value<min ) min=value;
            if( value>max ) max=value;
        }

        n_processed+=n_to_process;

        size_t perc=100*n_processed/n_total/2;
        emit percentDone(perc);
        qApp->processEvents();
    }

    return createHistogram( begin, end, NULL_VALUE, min, max, STEPS );
}


// this will show a progressDialog
template<typename ITERATOR, typename VALUE>
Histogram HistogramCreator::createHistogram( QString progressLabel, ITERATOR begin, ITERATOR end, VALUE NULL_VALUE, size_t STEPS){

    QProgressDialog pdlg;
    pdlg.setWindowTitle(tr("Computing Histogram"));
    pdlg.setLabelText(progressLabel);
    pdlg.setCancelButton(nullptr);
    pdlg.setRange(0,100);
    pdlg.setMinimumDuration(0);
    pdlg.show();
    connect( this, SIGNAL( percentDone(int)), &pdlg, SLOT(setValue(int)) );
    qApp->processEvents();

    return createHistogram( begin, end, NULL_VALUE, STEPS);
}

#endif // HISTOGRAMCREATOR_H
