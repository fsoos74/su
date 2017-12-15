#include "griditem.h"

#include<QColor>
#include<QPen>
#include<alignedtextgraphicsitem.h>

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

    if( m_colorTable ) m_colorTable->setRange(valueRange(*m_grid));

    //updatePixmap(); triggere3d from color table

    //updatePixmap();
    updateGeometry();
    updateFrame();
    updateLabels();
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

void GridItem::setShowMesh(bool on){
    if( on==m_mesh) return;
    m_mesh=on;
    updateMesh();
}

void GridItem::setShowLabels(bool on){
    if( on==m_labels) return;
    m_labels=on;
    updateLabels();
}

void GridItem::setInlineIncrement(int i){
    if( i==m_inlineIncrement) return;
    m_inlineIncrement=i;
    updateMesh();
    updateLabels();
}

void GridItem::setCrosslineIncrement(int i){
    if( i==m_crosslineIncrement) return;
    m_crosslineIncrement=i;
    updateMesh();
    updateLabels();
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

    QPen framePen( Qt::black,2);
    framePen.setCosmetic(true);
    auto f = new QGraphicsRectItem( 0, 0, bounds.height(), bounds.width(), this);
    f->setPen(framePen);
    m_frameItem=f;
}

void GridItem::updateLabels(){

    // delete old labels
    for( auto p : m_labelItems ){
        delete p;
    }
    m_labelItems.clear();

    if( !m_labels) return;
    if( !m_grid ) return;
    auto bounds=m_grid->bounds();

    // add inline labels on both sides
    auto i1=bounds.i1()/m_inlineIncrement;
    auto i2=bounds.i2()/m_inlineIncrement;

    for( auto i=i1; i<=i2; i++){

        auto ix=(i-i1)*m_inlineIncrement;
        auto il=i*m_inlineIncrement;
        QString text=QString(" il%1 ").arg(il);

        // left label

        auto il1=new AlignedTextGraphicsItem(text, this);
        il1->setHAlign(Qt::AlignRight);
        il1->setVAlign(Qt::AlignVCenter);
        il1->setPos(ix, 0);
        il1->setRotation(90);
        m_labelItems.push_back(il1);

        // right label
        auto il2=new AlignedTextGraphicsItem(text, this);
        il2->setHAlign(Qt::AlignLeft);
        il2->setVAlign(Qt::AlignVCenter);
        il2->setPos(ix, bounds.width());
        il2->setRotation(90);
        m_labelItems.push_back(il2);

    }


    // add crossline labels on both sides
    auto j1=bounds.j1()/m_crosslineIncrement;
    auto j2=bounds.j2()/m_crosslineIncrement;

    for( auto j=j1; j<=j2; j++){

        auto iy=(j-j1)*m_crosslineIncrement;
        auto xl=j*m_crosslineIncrement;
        QString text=QString(" xl%1 ").arg(xl);

        // bottom label
        auto xl1=new AlignedTextGraphicsItem(text, this);
        xl1->setRotation(0);
        xl1->setPos( 0 , iy );
        xl1->setHAlign(Qt::AlignRight);
        xl1->setVAlign(Qt::AlignVCenter);
        m_labelItems.push_back(xl1);

        // top label
        auto xl2=new AlignedTextGraphicsItem(text, this);
        xl2->setHAlign(Qt::AlignLeft);
        xl2->setVAlign(Qt::AlignVCenter);
        xl2->setRotation(0);
        xl2->setPos(bounds.height(), iy);
        m_labelItems.push_back(xl2);
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

    auto i1=bounds.i1()/m_inlineIncrement;
    auto i2=bounds.i2()/m_inlineIncrement;
    auto j1=bounds.j1()/m_crosslineIncrement;
    auto j2=bounds.j2()/m_crosslineIncrement;

    // add crosses
    const int S=2;
    QPen meshPen( Qt::black, 1);
    meshPen.setCosmetic(true);
    for( auto i=i1; i<=i2; i++){
        auto x=(i-i1)*m_inlineIncrement;
        for( auto j=j1; j<=j2; j++){
            auto y=(j-j1)*m_crosslineIncrement;
            auto l1=new QGraphicsLineItem( x-S, y, x+S, y, this);
            l1->setPen(meshPen);
            m_meshItems.push_back(l1);
            auto l2=new QGraphicsLineItem( x, y-S, x, y+S, this);
            l2->setPen(meshPen);
            m_meshItems.push_back(l2);
        }
    }
}
