#include "seismicdataselector.h"
#include "ui_seismicdataselector.h"

#include<QIntValidator>

const QString ORDER_NONE_STR("none");
const QString ILINE_ASC_STR("inline ascending");
const QString ILINE_DESC_STR("inline descending");
const QString XLINE_ASC_STR("crossline ascending");
const QString XLINE_DESC_STR("crossline descending");
const QString OFFSET_ASC_STR("offset ascending");
const QString OFFSET_DESC_STR("offset descending");

const int ORDER_NONE_INDEX=0;
const int ILINE_ASC_INDEX=1;
const int ILINE_DESC_INDEX=2;
const int XLINE_ASC_INDEX=3;
const int XLINE_DESC_INDEX=4;
const int OFFSET_ASC_INDEX=5;
const int OFFSET_DESC_INDEX=6;


SeismicDataSelector::SeismicDataSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeismicDataSelector)
{
    ui->setupUi(this);

    QStringList order;
    order.append(ORDER_NONE_STR);
    order.append(ILINE_ASC_STR);
    order.append(ILINE_DESC_STR);
    order.append( XLINE_ASC_STR);
    order.append(XLINE_DESC_STR);
    order.append(OFFSET_ASC_STR);
    order.append(OFFSET_DESC_STR);


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


QString SeismicDataSelector::selectionDescription(){

    if( !m_reader ) return QString();

    QString name=m_reader->info().name();

    GatherSortKey psort=primarySort();
    if( psort==GatherSortKey::Inline){
        return QString("%1 - inline %2 (%3)").arg(name).arg(ui->sbInline->value()).arg(ui->sbIlineCount->value());
    }
    else if( psort==GatherSortKey::Crossline){
        return QString("%1 - crossline %2 (%3)").arg(name).arg(ui->sbXline->value()).arg(ui->sbXlineCount->value());
    }
    else{
        return QString("%1 - random line").arg(name);
    }
}



void SeismicDataSelector::updatePrimarySort(){


    GatherSortKey key=GatherSortKey::None;

    switch(ui->cbOrder1->currentIndex()){

    case ILINE_ASC_INDEX:
    case ILINE_DESC_INDEX:
                key=GatherSortKey::Inline;
                break;
    case XLINE_ASC_INDEX:
    case XLINE_DESC_INDEX:
                key=GatherSortKey::Crossline;
                break;
    case OFFSET_ASC_INDEX:
    case OFFSET_DESC_INDEX:
                key=GatherSortKey::Offset;
                break;
    default:
                key=GatherSortKey::None;
                break;
    }

    setPrimarySort(key);
}

void SeismicDataSelector::setPrimarySort(GatherSortKey key){

    if( key==m_primarySortKey ) return;

    m_primarySortKey=key;

    emit primarySortChanged(key);
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

// smart version
void SeismicDataSelector::providePoint(int iline, int xline){

   if( primarySort()==GatherSortKey::Inline){
        ui->sbInline->setValue(iline);
   }
   else if( primarySort()==GatherSortKey::Crossline){
        ui->sbXline->setValue(xline);
   }
   else{
       ui->sbInline->setValue(iline);
       ui->sbXline->setValue(xline);
   }
}

/*
 * void SeismicDataSelector::providePoint(int iline, int xline){

   ui->sbInline->setValue(iline);
   ui->sbXline->setValue(xline);
}
*/

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

    emit descriptionChanged( selectionDescription() );
}


void SeismicDataSelector::provideRandomLine(QVector<QPoint> polyline){
//std::cout<<"SeismicDataSelector::provideRandomLine"<<std::endl;
//std::cout<<"#points="<<polyline.size()<<std::endl;
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
//std::cout<<"adding trace il="<<point.x()<<" xl="<<point.y()<<std::endl;
        //std::shared_ptr<seismic::Trace> trace=m_reader->readFirstTrace("iline", QString::number(point.x()),
        //                                                               "xline", QString::number(point.y()));
        std::shared_ptr<seismic::Trace> trace=m_reader->readFirstTrace("iline", QString::number(point.y()),         // changed this inline is y???
                                                                       "xline", QString::number(point.x()));
        if( !trace ){   // need error message!!!
            std::cerr<<"reading trace failed!"<<std::endl;
            continue;
        }

        gather->push_back(*trace);
    }

    m_gather=gather;
//std::cout<<"gather size="<<m_gather->size()<<std::endl;
    emit gatherChanged(m_gather);
}

void SeismicDataSelector::indexToKey( int idx, QString& key, bool& ascending){

    ascending=false;

    switch(idx){

    case ILINE_ASC_INDEX:
                ascending=true;
    case ILINE_DESC_INDEX:
                key="iline";
                break;
    case XLINE_ASC_INDEX:
                ascending=true;
    case XLINE_DESC_INDEX:
                key="xline";
                break;
    case OFFSET_ASC_INDEX:
                ascending=true;
    case OFFSET_DESC_INDEX:
                key="offset";
                break;

    default:
        key="";
    }
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

    updatePrimarySort();

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
    static int saveIndex1;
    static int saveIndex2;
    static int saveIndex3;

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

    if(checked){    // for random lines there is no primary sort order

        saveIndex1=ui->cbOrder1->currentIndex();
        saveIndex2=ui->cbOrder2->currentIndex();
        saveIndex3=ui->cbOrder3->currentIndex();
        ui->cbOrder1->setCurrentIndex(ORDER_NONE_INDEX);
        ui->cbOrder2->setCurrentIndex(ORDER_NONE_INDEX);
        ui->cbOrder3->setCurrentIndex(ORDER_NONE_INDEX);
    }
    else{       // restore old sort
        ui->cbOrder1->setCurrentIndex(saveIndex1);
        ui->cbOrder2->setCurrentIndex(saveIndex2);
        ui->cbOrder3->setCurrentIndex(saveIndex3);
    }
}

void SeismicDataSelector::on_tbInline_clicked()
{
    ui->cbOrder1->setCurrentIndex(ILINE_ASC_INDEX);   // inline ascending
    ui->cbOrder2->setCurrentIndex(XLINE_ASC_INDEX);   // xline ascending

    ui->sbInline->setValue( m_reader->minInline());
    ui->sbIlineCount->setValue(1);

    if(m_reader->info().mode()==SeismicDatasetInfo::Mode::Poststack ){
        ui->sbXline->setValue(m_reader->minCrossline());
        ui->sbXlineCount->setValue(m_reader->maxCrossline() - m_reader->minCrossline() + 1 );
        ui->cbOrder3->setCurrentIndex( ORDER_NONE_INDEX );
    }
    else{
        ui->sbXline->setValue(m_reader->minCrossline());
        ui->sbXlineCount->setValue( 10 );
        ui->cbOrder3->setCurrentIndex(OFFSET_ASC_INDEX);   // offset ascending

    }
}

void SeismicDataSelector::on_tbXLine_clicked()
{
    ui->cbOrder1->setCurrentIndex(XLINE_ASC_INDEX);   // xline ascending
    ui->cbOrder2->setCurrentIndex(ILINE_ASC_INDEX);   // inline ascending

    ui->sbXline->setValue( m_reader->minCrossline());
    ui->sbXlineCount->setValue(1);

    if(m_reader->info().mode()==SeismicDatasetInfo::Mode::Poststack ){
        ui->sbInline->setValue(m_reader->minInline());
        ui->sbIlineCount->setValue(m_reader->maxInline() - m_reader->minInline() + 1 );
        ui->cbOrder3->setCurrentIndex(ORDER_NONE_INDEX);
    }
    else{
        ui->sbInline->setValue(m_reader->minInline());
        ui->sbIlineCount->setValue( 10 );
        ui->cbOrder3->setCurrentIndex(OFFSET_ASC_INDEX);
    }


}
