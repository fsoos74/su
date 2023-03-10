#include "griditem.h"

#include<QColor>
#include<QPen>
#include<QFont>
#include<alignedtextgraphicsitem.h>
#include<cmath>
#include<histogram.h>

GridItem::GridItem(AVOProject* project, QGraphicsItem* parentGraphicsItemItem, QObject* parentQObject )
 : QObject( parentQObject), QGraphicsPixmapItem(parentGraphicsItemItem), m_project(project)
{
    m_colorTable = new ColorTable(this);
    m_colorTable->setColors(ColorTable::defaultColors());
    connect( m_colorTable, SIGNAL(colorsChanged()), this, SLOT(updatePixmap()) );
    connect( m_colorTable, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(updatePixmap()) );
    connect( m_colorTable, SIGNAL(powerChanged(double)), this, SLOT(updatePixmap()) );

    setTransformationMode(Qt::SmoothTransformation);
}

void GridItem::setProject(AVOProject* p){

    if( p==m_project ) return;

    m_project=p;

    updateGeometry();
}

void GridItem::setGrid(std::shared_ptr<Grid2D<float> > g){

    if( g==m_grid ) return;

    m_grid=g;

    emit dataChanged(m_grid->data(), m_grid->size(), m_grid->NULL_VALUE);

    if(m_colorTable && !m_colorTable->isLocked()){
        /*
        // this is for 3 sigma, works well for volumes but not good for horizons, so leave it
        Histogram h=createHistogram(m_grid->cbegin(),m_grid->cend(),m_grid->NULL_VALUE,1);	// only for stats
        auto rmin=h.mean()-3*h.sigma();
        auto rmax=h.mean()+3*h.sigma();
        m_colorTable->setRange(rmin,rmax);
        */
        m_colorTable->setRange(valueRange(*m_grid));	// min/max scaling
    }
    else{
        updatePixmap(); //triggere3d from color table on change
    }

    updateGeometry();
    updateFrame();
    updateLabel();
    updateLineLabels();
    updateMesh();
}

void GridItem::setColorTable( ColorTable* ct){

    if( ct == m_colorTable ) return;

    if( m_colorTable ){
        //disconnect old color table
        disconnect(this, SLOT(updatePixmap()) );
    }

    m_colorTable=ct;

    if( ct ){
        // redraw on colortable + scaling changes
        connect( ct, SIGNAL(colorsChanged()), this, SLOT(updatePixmap()) );
        connect( ct, SIGNAL(rangeChanged(std::pair<double,double>)), this, SLOT(updatePixmap()) );
        connect( ct, SIGNAL(powerChanged(double)), this, SLOT(updatePixmap()) );
    }
}

void GridItem::setLabel(QString l){
    if(l==m_label) return;
    m_label=l;
    updateLabel();
}

void GridItem::setShowMesh(bool on){
    if( on==m_mesh) return;
    m_mesh=on;
    updateMesh();
}

void GridItem::setShowLineLabels(bool on){
    if( on==m_showLineLabels) return;
    m_showLineLabels=on;
    updateLineLabels();
}

void GridItem::setShowLabel(bool on){
    if( on==m_showLabel) return;
    m_showLabel=on;
    updateLabel();
}

void GridItem::setInlineIncrement(int i){
    if( i==m_inlineIncrement) return;
    m_inlineIncrement=i;
    updateMesh();
    updateLineLabels();
}

void GridItem::setCrosslineIncrement(int i){
    if( i==m_crosslineIncrement) return;
    m_crosslineIncrement=i;
    updateMesh();
    updateLineLabels();
}

void GridItem::setZValueWrapper(int i){
    if(i==zValue()) return;
    setZValue(i);
}

void GridItem::setOpacityWrapper(double x){
    if(x==opacity()) return;
    setOpacity(x);
}

void GridItem::updatePixmap(){

    if( !m_grid ){
        setPixmap(QPixmap());
        return;
    }

    auto range=valueRange(*m_grid);
    if( range.first==range.second) return;

    Grid2DBounds bounds=m_grid->bounds();
    QImage img(bounds.height(), bounds.width(), QImage::Format_RGBA8888);
    img.fill(qRgba(255,255,255,0));  // make transparent, for NULL values pixels will not be set

    for( auto i = bounds.i1(); i<=bounds.i2(); i++){

        for( auto j=bounds.j1(); j<=bounds.j2(); j++){

            auto value=m_grid->valueAt(i,j);
            if( value==m_grid->NULL_VALUE) continue;
            auto col = m_colorTable->map(value);
            //auto rel= (value-range.first)/(range.second-range.first);
            //auto col=qRgb( 255*rel, 255*rel, 255*rel);

            img.setPixel( i-bounds.i1(), j-bounds.j1(), col );
       }
    }

   setPixmap( QPixmap::fromImage(img));
}

void GridItem::updateGeometry(){

    if( !m_project ) return;

    auto bounds=m_grid->bounds();

    QTransform tf;
    tf.translate( bounds.i1(), bounds.j1());

    tf.scale(qreal(bounds.height()-1)/bounds.height(),
            qreal(bounds.width()-1)/bounds.width() );

    QTransform ilxl_to_xy, xy_to_ilxl;
    m_project->geometry().computeTransforms( xy_to_ilxl, ilxl_to_xy );
    tf=tf*ilxl_to_xy;

    setTransform(tf);
}

void GridItem::updateFrame(){

    if( m_frameItem) delete m_frameItem;

    if( !m_grid ) return;

    auto bounds=m_grid->bounds();

    QPen framePen( Qt::black,1);
    framePen.setCosmetic(true);
    auto f = new QGraphicsRectItem( 0, 0, bounds.height(), bounds.width(), this);
    f->setPen(framePen);
    m_frameItem=f;
}

void GridItem::updateLabel(){
    // delete old label
    if(m_labelItem){
        delete m_labelItem;
        m_labelItem=nullptr;
    }
    if(!m_showLabel) return;
    if( !m_grid ) return;
    auto bounds=m_grid->bounds();
    auto az=(m_project)?m_project->geometry().azimuth() : 0.;
    auto l=new AlignedTextGraphicsItem(m_label, Qt::AlignBottom|Qt::AlignHCenter,az,this);
    l->setPos(0,bounds.width()/2);
    l->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    QFont font("Times",8,QFont::Normal);
    l->setFont(font);
    m_labelItem=l;
}

void GridItem::updateLineLabels(){

    // delete old labels
    for( auto p : m_lineLabelItems ){
        delete p;
    }
    m_lineLabelItems.clear();

    if( !m_showLineLabels) return;
    if( !m_grid ) return;
    QFont font("Times",8,QFont::Normal);
    auto bounds=m_grid->bounds();
    auto az=(m_project)?m_project->geometry().azimuth() : 0.;

    // add inline labels on both sides
    auto i1=static_cast<int>(std::ceil(double(bounds.i1())/m_inlineIncrement));
    auto i2=static_cast<int>(std::floor(double(bounds.i2())/m_inlineIncrement));

    for( auto i=i1; i<=i2; i++){

        auto il=i*m_inlineIncrement;
        auto ix=il-bounds.i1();
        QString text=QString(" il%1").arg(il);

        // left label
        auto il1=new AlignedTextGraphicsItem(text, Qt::AlignRight|Qt::AlignVCenter,az,this);
        il1->setPos(ix, 0);
        il1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        il1->setFont(font);
        m_lineLabelItems.push_back(il1);

        // right label
        auto il2=new AlignedTextGraphicsItem( text, Qt::AlignLeft|Qt::AlignVCenter,az,this);
        il2->setPos(ix, bounds.width());
        il2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        il2->setFont(font);
        m_lineLabelItems.push_back(il2);
    }


    // add crossline labels on both sides
    auto j1=static_cast<int>(std::ceil(double(bounds.j1())/m_crosslineIncrement));
    auto j2=static_cast<int>(std::floor(double(bounds.j2())/m_crosslineIncrement));

    for( auto j=j1; j<=j2; j++){

        auto xl=j*m_crosslineIncrement;
        auto iy=xl-bounds.j1();
        QString text=QString(" xl%1").arg(xl);

        // bottom label
        auto xl1=new AlignedTextGraphicsItem(text, Qt::AlignVCenter|Qt::AlignLeft,az+90,this);
        xl1->setPos(0,iy);
        xl1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        xl1->setFont(font);
        m_lineLabelItems.push_back(xl1);

        // top label
        auto xl2=new AlignedTextGraphicsItem(text, Qt::AlignVCenter|Qt::AlignRight,az+90,this);
        xl2->setPos(bounds.height(),iy);
        xl2->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        xl2->setFont(font);
        m_lineLabelItems.push_back(xl2);
    }

}


void GridItem::updateMesh(){

    // delete old mesh items
    for( auto p : m_meshItems ){
        delete p;
    }
    m_meshItems.clear();

    if( !m_mesh ) return;
    if( !m_grid ) return;
    auto bounds=m_grid->bounds();

    auto i1=static_cast<int>(std::ceil(double(bounds.i1())/m_inlineIncrement));
    auto i2=static_cast<int>(std::floor(double(bounds.i2())/m_inlineIncrement));
    auto j1=static_cast<int>(std::ceil(double(bounds.j1())/m_crosslineIncrement));
    auto j2=static_cast<int>(std::floor(double(bounds.j2())/m_crosslineIncrement));

    // add crosses
    const int S=2;
    QPen meshPen( Qt::black, 1);
    meshPen.setCosmetic(true);
    for( auto i=i1; i<=i2; i++){
        auto x=i*m_inlineIncrement-bounds.i1();
        for( auto j=j1; j<=j2; j++){
            auto y=j*m_crosslineIncrement-bounds.j1();
            auto l1=new QGraphicsLineItem( x-S, y, x+S, y, this);
            l1->setPen(meshPen);
            m_meshItems.push_back(l1);
            auto l2=new QGraphicsLineItem( x, y-S, x, y+S, this);
            l2->setPen(meshPen);
            m_meshItems.push_back(l2);
        }
    }
}
