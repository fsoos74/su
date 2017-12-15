#include "spectrumview.h"

#include<QPainterPath>
#include<QGraphicsPathItem>
#include<QGraphicsEllipseItem>

SpectrumView::SpectrumView(QWidget* parent) :RulerAxisView(parent)
{

}

#include<iostream>
void SpectrumView::setSpectrum(std::shared_ptr<Spectrum> s){

    if( s==m_spectrum) return;

    m_spectrum=s;

    xAxis()->setRange(s->fmin(), s->fmax());
    xAxis()->setViewRange(s->fmin(), s->fmax());
    // scan amplitude range, better add functions amplitude rane, frequency range and phase range to spectrum
    auto amin=std::numeric_limits<Spectrum::float_type>::max();
    auto amax=std::numeric_limits<Spectrum::float_type>::lowest();
    for( Spectrum::index_type i=0; i<s->size(); i++ ){
        auto a = s->amplitude(i);
        //std::cout<<i<<" -> "<<a<<std::endl;
        if(a<amin) amin=a;
        if(a>amax) amax=a;
    }

    std::cout<<"Spectrum size="<<s->size()<<std::endl<<std::flush;
    std::cout<<"Spectrum frq range: "<<s->fmin()<<" - "<<s->fmax()<<std::endl<<std::flush;
    std::cout<<"Spectrum amp range: "<<amin<<" - "<<amax<<std::endl<<std::flush;

    if( amin<amax ){
        zAxis()->setRange(amin,amax);
        zAxis()->setViewRange(amin,amax);
    }

    refreshScene();

    fitInView(sceneRect());
}


void SpectrumView::refreshScene(){

    QGraphicsScene* scene=new QGraphicsScene(this);

    if( m_spectrum ){

        /*
        // draw spectrum
        auto path=buildPath(*m_spectrum);
        QGraphicsPathItem* gitem=new QGraphicsPathItem(path);
        gitem->setPen( QPen( Qt::darkBlue, 0));
        scene->addItem( gitem );
        */
        const Spectrum& s=*m_spectrum;
        for( Spectrum::index_type i=0; i<s.size(); i++){

            auto z = s.amplitude(i);
            auto x = s.frequency(i);
            x=xAxis()->toScene(x);
            z=zAxis()->toScene(z);

            QGraphicsEllipseItem* item=new QGraphicsEllipseItem(-2,-2,4,4);
            item->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

            item->setPos(x,z);
            scene->addItem(item);
        }
    }

    setScene(scene);
}


QPainterPath SpectrumView::buildPath(const Spectrum& s){

    QPainterPath path;

    for( Spectrum::index_type i=0; i<s.size(); i++){

        auto z = s.amplitude(i);
        auto x = s.frequency(i);
        x=xAxis()->toScene(x);
        z=zAxis()->toScene(z);

        QPointF p(x,z);
        if( i>0 ){
            path.lineTo(p);
        }
        else{
            path.moveTo(p);
        }
    }

    return path;
}
