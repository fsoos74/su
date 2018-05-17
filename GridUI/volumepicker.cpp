#include "volumepicker.h"

#include<limits>
#include<tuple>
#include<QList>
#include<QProgressDialog>
#include<QApplication>     // processevents
#include<QMap>


namespace{
QMap<VolumePicker::PickMode, QString> lookup{
    { VolumePicker::PickMode::Points, "Points"},
    {VolumePicker::PickMode::Lines, "Lines"},
    {VolumePicker::PickMode::Outline, "Outline"}
};

}

QString toQString(VolumePicker::PickMode m){
    return lookup.value(m, "Points");
}

VolumePicker::PickMode toVolumePickMode(QString str){
    return lookup.key(str, VolumePicker::PickMode::Points);
}

QStringList volumePickModeNames(){
    return lookup.values();
}

QList<VolumePicker::PickMode> volumePickModes(){
    return lookup.keys();
}

using namespace std::placeholders;

namespace{

double lininterp(double x1, double y1, double x2, double y2, double x){

    // need to add eps checks instead ==


   // if( x<=x1 ) return y1;
   // if( x>=x2 ) return y2;
    if( x1 == x2 ) return (y1+y2)/2;

    return y1 + x * ( y2 - y1 ) / ( x2 - x1 );
}

}


VolumePicker::VolumePicker(VolumeView *view) : QObject(view), m_view(view), m_dirty(false), m_pickMode(PickMode::Points)
{
    m_picks=std::make_shared<Table>("iline", "xline", true);
    view->setSelectionMode(VolumeView::SELECTIONMODE::SinglePoint);
    connect(view, SIGNAL(pointSelected(QPointF)), this, SLOT(pickPoint(QPointF)));
    connect(view, SIGNAL(removePoint(QPointF)), this, SLOT(removePoint(QPointF)));
    connect(this, SIGNAL(picksChanged()), view, SLOT(update()));
}


void VolumePicker::newPicks(){

    m_picks=std::make_shared<Table>("iline", "xline", true);
    setDirty(false);
    emit picksChanged();
}

void VolumePicker::setPicks( std::shared_ptr<Table> p ){

    if( m_picks == p ) return;
    m_picks=p;
    setDirty(false);
    emit picksChanged();
}

void VolumePicker::setPickMode(PickMode m){
    if( m==m_pickMode) return;
    m_pickMode=m;
    disconnect(m_view, SIGNAL(pointSelected(QPointF)), 0, 0);
    disconnect(m_view, SIGNAL(polygonSelected(QPolygonF)), 0, 0);
    switch( m_pickMode){
    case PickMode::Points:
        m_view->setSelectionMode(VolumeView::SELECTIONMODE::SinglePoint);
        connect( m_view, SIGNAL(pointSelected(QPointF)), this, SLOT(pickPoint(QPointF)) );
        break;
    case PickMode::Lines:
        m_view->setSelectionMode(VolumeView::SELECTIONMODE::Lines);
        connect( m_view, SIGNAL(linesSelected(QPolygonF)), this, SLOT(pickLines(QPolygonF)));
        break;
    case PickMode::Outline:
        m_view->setSelectionMode(VolumeView::SELECTIONMODE::Polygon);
        connect( m_view, SIGNAL(polygonSelected(QPolygonF)), this, SLOT(pickPolygon(QPolygonF)));
        break;
    }
}

void VolumePicker::setPickMode(QString str){
    setPickMode(toVolumePickMode(str));
}

//#include<iostream>
void VolumePicker::pickPoint(QPointF p){
    int il=0;
    int xl=0;
    float t=0;
    auto bounds=m_view->bounds();
    auto sdef=m_view->slice();
    switch( sdef.type){
    case VolumeView::SliceType::Inline:
        il=sdef.value;
        xl=std::round(p.x());
        t=0.001*p.y();
        break;
    case VolumeView::SliceType::Crossline:
        il=std::round(p.x());
        xl=sdef.value;
        t=0.001*p.y();
        break;
    case VolumeView::SliceType::Z:
        il=std::round(p.x());
        xl=std::round(p.y());
        t=0.001*sdef.value;     // sdef is in millis
        break;
    }
    //std::cout<<"Pick il="<<il<<" xl="<<xl<<" t="<<t<<std::endl<<std::flush;
    m_picks->insert(il,xl,t);
    setDirty(true);
    emit picksChanged();
}


void VolumePicker::pickLines(QPolygonF poly){

    switch(m_view->slice().type){
    case VolumeView::SliceType::Inline: fillLinesIline(poly);break;
    case VolumeView::SliceType::Crossline: fillLinesXline(poly);break;
    case VolumeView::SliceType::Z: fillLinesZ(poly);break;
    }
}

void VolumePicker::fillLinesIline(QPolygonF poly){

    QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> buffer;    // buffer picks and add them all at once later
    auto il=m_view->slice().value;
    auto bounds=m_view->bounds();

    for( int i=1; i<poly.size(); i++){
        auto p0=poly.at(i-1);
        auto xl0=static_cast<int>(std::round(p0.x()));
        auto t0=0.001*p0.y();   // msec -> sec
        auto sam0=bounds.timeToSample(t0);
        auto p1=poly.at(i);
        auto xl1=static_cast<int>(std::round(p1.x()));
        auto t1=0.001*p1.y();   // msec -> sevc
        auto sam1=bounds.timeToSample(t1);

        if( std::abs(xl1-xl0)>std::abs(sam1-sam0)){
            if(xl1<xl0){
                std::swap(xl0,xl1);
                std::swap(t0,t1);
            }

            double m=(t1-t0)/(xl1-xl0);
            for(int xl=xl0; xl<=xl1; xl++){
                auto t=t0+(xl-xl0)*m;
                buffer.push_back(std::make_tuple(il, xl, t));
            }
        }
        else{
            if(sam1<sam0){
                std::swap(sam1,sam0);
                std::swap(xl1,xl0);
            }

            double m=double(xl1-xl0)/(sam1-sam0);
            for( int sam=sam0; sam<=sam1; sam++){
                auto t=bounds.sampleToTime(sam);
                auto xl = static_cast<int>(std::round(xl0+(sam-sam0)*m));
                buffer.push_back(std::make_tuple(il, xl, t));
            }
        }

    }

    m_picks->insert(buffer);
    setDirty(true);
    emit picksChanged();
}

void VolumePicker::fillLinesXline(QPolygonF poly){

    QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> buffer;    // buffer picks and add them all at once later
    auto xl=m_view->slice().value;
    auto bounds=m_view->bounds();

    for( int i=1; i<poly.size(); i++){
        auto p0=poly.at(i-1);
        auto il0=static_cast<int>(std::round(p0.x()));
        auto t0=0.001*p0.y();   // msec -> sec
        auto sam0=bounds.timeToSample(t0);
        auto p1=poly.at(i);
        auto il1=static_cast<int>(std::round(p1.x()));
        auto t1=0.001*p1.y();   // msec -> sevc
        auto sam1=bounds.timeToSample(t1);

        if( std::abs(il1-il0)>std::abs(sam1-sam0)){
            if(il1<il0){
                std::swap(il0,il1);
                std::swap(t0,t1);
            }

            double m=(t1-t0)/(il1-il0);
            for(int il=il0; il<=il1; il++){
                auto t=t0+(il-il0)*m;
                buffer.push_back(std::make_tuple(il, xl, t));
            }
        }
        else{
            if(sam1<sam0){
                std::swap(sam1,sam0);
                std::swap(il1,il0);
            }

            double m=double(il1-il0)/(sam1-sam0);
            for( int sam=sam0; sam<=sam1; sam++){
                auto t=bounds.sampleToTime(sam);
                auto il = static_cast<int>(std::round(il0+(sam-sam0)*m));
                buffer.push_back(std::make_tuple(il, xl, t));
            }
        }

    }

    m_picks->insert(buffer);
    setDirty(true);
    emit picksChanged();
}

void VolumePicker::fillLinesZ(QPolygonF poly){

    QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> buffer;    // buffer picks and add them all at once later
    auto t=0.001*m_view->slice().value;

    for( int i=1; i<poly.size(); i++){
        auto p0=poly.at(i-1);
        auto il0=static_cast<int>(std::round(p0.x()));
        auto xl0=static_cast<int>(std::round(p0.y()));
        auto p1=poly.at(i);
        auto il1=static_cast<int>(std::round(p1.x()));
        auto xl1=static_cast<int>(std::round(p1.y()));

        if( std::abs(il1-il0)>std::abs(xl1-xl0)){
            if(il1<il0){
                std::swap(il0,il1);
                std::swap(xl0,xl1);
            }
            double m=double(xl1-xl0)/(il1-il0);
            for( auto il=il0; il<=il1; il++){
                int xl = static_cast<int>(std::round(xl0 + (il-il0)*m));
                buffer.push_back(std::make_tuple(il, xl, t));
            }
        }
        else{
            if(xl1<xl0){
                std::swap(il0,il1);
                std::swap(xl0,xl1);
            }
            double m=double(il1-il0)/(xl1-xl0);
            for( auto xl=xl0; xl<=xl1; xl++){
                int il = static_cast<int>(std::round(il0 + (xl-xl0)*m));
                buffer.push_back(std::make_tuple(il, xl, t));
            }
        }

    }

    m_picks->insert(buffer);
    setDirty(true);
    emit picksChanged();


}


void VolumePicker::pickPolygon(QPolygonF poly){

    switch(m_view->slice().type){
    case VolumeView::SliceType::Inline: fillOutlineIline(poly);break;
    case VolumeView::SliceType::Crossline: fillOutlineXline(poly);break;
    case VolumeView::SliceType::Z: fillOutlineZ(poly);break;
    }
}

void VolumePicker::fillOutlineIline(QPolygonF poly){
    auto bb=poly.boundingRect();
    auto xl0=static_cast<int>( std::floor(bb.left()));
    auto xl1=static_cast<int>( std::ceil(bb.right()));
    auto t0=0.001*bb.top(); // msec -> sec
    auto t1=0.001*bb.bottom();  // msec->sec

    // fill outline with picks
    QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> buffer;    // buffer picks and add them all at once later

    QProgressDialog progress("Adding picks...", "Cancel", xl0, xl1, dynamic_cast<QWidget*>(parent()) );
    progress.setMinimum(xl0);
    progress.setMaximum(xl1);
    progress.setValue(xl0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setWindowTitle("Filling Outline");
    progress.setUpdatesEnabled(true);
    progress.show();
    qApp->processEvents();

    auto bounds=m_view->bounds();
    auto i0=bounds.timeToSample(t0);
    auto i1=bounds.timeToSample(t1);

    auto il=m_view->slice().value;

    for( int xl=xl0; xl<xl1; xl++){

        for( int i=i0; i<i1; i++){
            auto t=bounds.sampleToTime(i);
            if( poly.containsPoint(QPointF(xl, 1000.*t), Qt::OddEvenFill)){
                buffer.push_back(std::make_tuple(il, xl, t));
            }
        }
        progress.setValue(xl);
        qApp->processEvents();
        if( progress.wasCanceled()) return;
    }

    m_picks->insert(buffer);
    progress.setValue(xl1);

    setDirty(true);
    emit picksChanged();
}

void VolumePicker::fillOutlineXline(QPolygonF poly){
    auto bb=poly.boundingRect();
    auto il0=static_cast<int>( std::floor(bb.left()));
    auto il1=static_cast<int>( std::ceil(bb.right()));
    auto t0=0.001*bb.top(); // msec -> sec
    auto t1=0.001*bb.bottom();  // msec->sec

    // fill outline with picks
    QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> buffer;    // buffer picks and add them all at once later

    QProgressDialog progress("Adding picks...", "Cancel", il0, il1, dynamic_cast<QWidget*>(parent()) );
    progress.setMinimum(il0);
    progress.setMaximum(il1);
    progress.setValue(il0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setWindowTitle("Filling Outline");
    progress.setUpdatesEnabled(true);
    progress.show();
    qApp->processEvents();

    auto bounds=m_view->bounds();
    auto i0=bounds.timeToSample(t0);
    auto i1=bounds.timeToSample(t1);

    auto xl=m_view->slice().value;

    for( int il=il0; il<il1; il++){

        for( int i=i0; i<i1; i++){
            auto t=bounds.sampleToTime(i);
            if( poly.containsPoint(QPointF(il, 1000.*t), Qt::OddEvenFill)){
                buffer.push_back(std::make_tuple(il, xl, t));
            }
        }
        progress.setValue(il);
        qApp->processEvents();
        if( progress.wasCanceled()) return;
    }

    m_picks->insert(buffer);
    progress.setValue(il1);
    setDirty(true);
    emit picksChanged();
}


void VolumePicker::fillOutlineZ(QPolygonF poly){
    auto bb=poly.boundingRect();
    auto il0=static_cast<int>( std::floor(bb.left()));
    auto il1=static_cast<int>( std::ceil(bb.right()));
    auto xl0=bb.top();
    auto xl1=bb.bottom();

    // fill outline with picks
    QList<std::tuple<Table::key_t, Table::key_t, Table::value_t>> buffer;    // buffer picks and add them all at once later

    QProgressDialog progress("Adding picks...", "Cancel", il0, il1, dynamic_cast<QWidget*>(parent()) );
    progress.setMinimum(il0);
    progress.setMaximum(il1);
    progress.setValue(il0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setWindowTitle("Filling Outline");
    progress.setUpdatesEnabled(true);
    progress.show();
    qApp->processEvents();

    auto secs=0.001*m_view->slice().value;  // msec -> sec

    for( int il=il0; il<il1; il++){

        for( int xl=xl0; xl<xl1; xl++){
            if( poly.containsPoint(QPointF(il, xl), Qt::OddEvenFill)){
                buffer.push_back(std::make_tuple(il, xl, secs));
            }
        }
        progress.setValue(il);
        qApp->processEvents();
        if( progress.wasCanceled()) return;
    }

    m_picks->insert(buffer);
    progress.setValue(il1);
    setDirty(true);
    emit picksChanged();
}

void VolumePicker::removePoint(QPointF p){

    const int ap_il=11;
    const int ap_xl=11;
    const int ap_sam=11;

    int il0=0;
    int il1=0;
    int xl0=0;
    int xl1=0;
    float t0=0;
    float t1=0;
    auto bounds=m_view->bounds();
    auto sdef=m_view->slice();
    switch( sdef.type){
    case VolumeView::SliceType::Inline:{
        il0=il1=sdef.value;
        auto xl=std::round(p.x());
        xl0=xl-ap_xl/2;
        xl1=xl+ap_xl/2;
        auto t=0.001*p.y();
        t0=t-0.5*ap_sam*bounds.dt();
        t1=t+0.5*ap_sam*bounds.dt();
        break;
    }
    case VolumeView::SliceType::Crossline:{
        auto il=std::round(p.x());
        il0=il-ap_il/2;
        il1=il+ap_il/2;
        xl0=xl1=sdef.value;
        auto t=0.001*p.y();
        t0=t-0.5*ap_sam*bounds.dt();
        t1=t+0.5*ap_sam*bounds.dt();
        break;
    }
    case VolumeView::SliceType::Z:{
        auto il=std::round(p.x());
        il0=il-ap_il/2;
        il1=il+ap_il/2;
        auto xl=std::round(p.y());
        xl0=xl-ap_xl/2;
        xl1=xl+ap_xl/2;
        auto t=0.001*sdef.value;     // sdef is in millis
        t0=t-0.5*bounds.dt();
        t1=t+0.5*bounds.dt();
        break;
    }
    }

    for( int il=il0; il<=il1; il++){
        for(int xl=xl0; xl<=xl1; xl++){
            m_picks->removeValueRange(il,xl,t0,t1);
        }
    }
    setDirty(true);
    emit picksChanged();
}


void VolumePicker::setDirty(bool on){
    m_dirty=on;
}

