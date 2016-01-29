#include "seismicdataselector.h"
#include "ui_seismicdataselector.h"

#include<QIntValidator>

SeismicDataSelector::SeismicDataSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeismicDataSelector)
{
    ui->setupUi(this);
    QStringList order;
    order.append("none");
    order.append("inline ascending");
    order.append("inline descending");
    order.append("crossline ascending");
    order.append("crossline descending");
    order.append("offset ascending");
    order.append("offset descending");

    ui->cbOrder1->addItems(order);
    ui->cbOrder2->addItems(order);
    ui->cbOrder3->addItems(order);


    connect(ui->sbInline, SIGNAL(valueChanged(int)), this, SLOT(readGather()));
    connect(ui->sbXline, SIGNAL(valueChanged(int)), this, SLOT(readGather()));
    connect(ui->sbIlineCount, SIGNAL(valueChanged(int)), this, SLOT(readGather()));
    connect(ui->sbXlineCount, SIGNAL(valueChanged(int)), this, SLOT(readGather()));
    connect( ui->cbOrder1, SIGNAL(currentIndexChanged(int)), this, SLOT(changeOrder()));
    connect( ui->cbOrder2, SIGNAL(currentIndexChanged(int)), this, SLOT(changeOrder()));
    connect( ui->cbOrder3, SIGNAL(currentIndexChanged(int)), this, SLOT(changeOrder()));
}

SeismicDataSelector::~SeismicDataSelector()
{
    delete ui;
}

void SeismicDataSelector::apply(){
    readGather();
}


void SeismicDataSelector::setReader(std::shared_ptr<SeismicDatasetReader> reader){
    m_reader=reader;
}

void SeismicDataSelector::setInlineRange( int min, int max){
    ui->sbInline->setRange(min,max);
}

void SeismicDataSelector::setCrosslineRange( int min, int max){
    ui->sbXline->setRange(min,max);
}

void SeismicDataSelector::setInlineCountRange( int min, int max ){
    ui->sbIlineCount->setRange(min, max);
}

void SeismicDataSelector::setCrosslineCountRange( int min, int max ){
    ui->sbXlineCount->setRange(min, max);
}

void SeismicDataSelector::setInline( int val ){
    ui->sbInline->setValue(val);
}

void SeismicDataSelector::setCrossline( int val ){
    ui->sbXline->setValue(val);
}

void SeismicDataSelector::setInlineCount( int count ){
    ui->sbIlineCount->setValue(count);
}

void SeismicDataSelector::setCrosslineCount( int count ){
    ui->sbXlineCount->setValue(count);
}

void SeismicDataSelector::setOrder(int k1, int k2, int k3){

    ui->cbOrder1->setCurrentIndex(k1);
    ui->cbOrder2->setCurrentIndex(k2);
    ui->cbOrder3->setCurrentIndex(k3);
}

void SeismicDataSelector::providePoint(int iline, int xline){

   ui->sbInline->setValue(iline);
   ui->sbXline->setValue(xline);
}

void SeismicDataSelector::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}


void SeismicDataSelector::readGather(){

    if( !m_reader ) return;

    int minIline=ui->sbInline->value();
    int maxIline=minIline + ui->sbIlineCount->value()-1;    // minimum count=1
    int minXline=ui->sbXline->value();
    int maxXline=minXline + ui->sbXlineCount->value()-1;    // minimum count=1

    m_gather=m_reader->readGather("iline",QString::number(minIline),QString::number(maxIline),
                                "xline",QString::number(minXline),QString::number(maxXline),
                                  MAXIMUM_TRACES_PER_SCREEN);

    emit gatherChanged(m_gather);

}


void SeismicDataSelector::provideRandomLine(QVector<QPoint> polyline){

    if( !m_reader ) return;

    if( !ui->rbRandomLine->isChecked()) return;

    if( polyline.size()<2) return;

    // find all cdp/points on line
    QVector<QPoint> allPoints;
    for( int i=1; i<polyline.size(); i++){

        QPoint p1=polyline[i-1];
        QPoint p2=polyline[i];
        int dx=p2.x() - p1.x();
        int dy=p2.y() - p1.y();

        if( dx==0 ){
            if(dy>0){
                for( int y=p1.y(); y<p2.y(); y++){
                    allPoints.append(QPoint(p1.x(), y));
                }
            }
            else{
                for( int y=p1.y(); y>p2.y(); y--){
                    allPoints.append(QPoint(p1.x(), y));
                }
            }
        }
        else if( dy==0 ){
            if( dx>0){
                for( int x=p1.x(); x<p2.x();x++){
                    allPoints.append( QPoint(x, p1.y() ) );
                }
            }
            else{
                for( int x=p1.x(); x>p2.x();x--){
                    allPoints.append( QPoint(x, p1.y() ) );
                }
            }
        }
        else if( std::abs(dx)>std::abs(dy) ){
            if(dx>0){
                for( int x=p1.x(); x<p2.x();x++){
                    int y=p1.y() + ( x - p1.x() ) * dy / dx;
                    allPoints.append( QPoint(x, y ) );
                }
            }
            else{
                for( int x=p1.x(); x>p2.x();x--){
                    int y=p1.y() + ( x - p1.x() ) * dy / dx;
                    allPoints.append( QPoint(x, y ) );
                }
            }
        }
        else{
            if(dy>0){
                for( int y=p1.y(); y<p2.y(); y++){
                    int x=p1.x() + ( y - p1.y() ) * dx / dy;
                    allPoints.append(QPoint(x, y));
                }
            }
            else{
                for( int y=p1.y(); y>p2.y(); y--){
                    int x=p1.x() + ( y - p1.y() ) * dx / dy;
                    allPoints.append(QPoint(x, y));
                }
            }
        }
    }

    allPoints.append(polyline.last()); // dont forget endpoint of last segment

    // fill gather
    std::shared_ptr<seismic::Gather> gather( new seismic::Gather);
    for( QPoint point : allPoints){

        std::shared_ptr<seismic::Trace> trace=m_reader->readFirstTrace("iline", QString::number(point.x()),
                                                                       "xline", QString::number(point.y()));
        if( !trace ){   // need error message!!!
            continue;
        }

        gather->push_back(*trace);
    }

    m_gather=gather;

    emit gatherChanged(m_gather);
}

void SeismicDataSelector::indexToKey( int idx, QString& key, bool& ascending){

    if( idx>0){
        switch(idx){
        case 1:
        case 2:
                    key="iline";
                    break;
        case 3:
        case 4:
                    key="xline";
                    break;
        case 5:
        case 6:
                    key="offset";
                    break;

        }
    }
    else{
        key="";
    }

    ascending=((idx%2)==1);
}

void SeismicDataSelector::changeOrder(){

    QString key1;
    bool ascending1;
    indexToKey(ui->cbOrder1->currentIndex(), key1, ascending1);

    QString key2;
    bool ascending2;
    indexToKey(ui->cbOrder2->currentIndex(), key2, ascending2);

    QString key3;
    bool ascending3;
    indexToKey(ui->cbOrder3->currentIndex(), key3, ascending3);

    m_reader->setOrder(key1,ascending1, key2,ascending2, key3,ascending3);

    readGather();
}

void SeismicDataSelector::on_sbIlineCount_valueChanged(int arg1)
{
    ui->sbInline->setSingleStep(arg1);
}

void SeismicDataSelector::on_sbXlineCount_valueChanged(int arg1)
{
    ui->sbXline->setSingleStep(arg1);
}

void SeismicDataSelector::on_rbRandomLine_toggled(bool checked)
{
    ui->label->setEnabled(!checked);
    ui->label_2->setEnabled(!checked);
    ui->label_3->setEnabled(!checked);
    ui->sbInline->setEnabled(!checked);
    ui->sbIlineCount->setEnabled(!checked);
    ui->sbXline->setEnabled(!checked);
    ui->sbXlineCount->setEnabled(!checked);
    ui->cbOrder1->setEnabled(!checked);
    ui->cbOrder2->setEnabled(!checked);
    ui->cbOrder3->setEnabled(!checked);
}
