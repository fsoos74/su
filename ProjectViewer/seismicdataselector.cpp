#include "seismicdataselector.h"
#include "ui_seismicdataselector.h"

#include<QIntValidator>

const QString ORDER_NONE_STR("none");
const QString ILINE_ASC_STR("iline asc");
const QString ILINE_DESC_STR("iline desc");
const QString XLINE_ASC_STR("xline asc");
const QString XLINE_DESC_STR("xline desc");
const QString OFFSET_ASC_STR("offset asc");
const QString OFFSET_DESC_STR("offset desc");

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


    connect(ui->sbInline, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->sbXline, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->sbIlineCount, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->sbXlineCount, SIGNAL(valueChanged(int)), this, SLOT(apply()));
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


void SeismicDataSelector::first()
{
    int order1=ui->cbOrder1->currentIndex();

    switch( order1 ){
    case ILINE_ASC_INDEX: ui->sbInline->setValue(ui->sbInline->minimum()); break;
    case ILINE_DESC_INDEX: ui->sbInline->setValue(ui->sbInline->maximum()); break;
    case XLINE_ASC_INDEX: ui->sbXline->setValue(ui->sbXline->minimum()); break;
    case XLINE_DESC_INDEX: ui->sbXline->setValue(ui->sbXline->maximum()); break;
    default: //nop
        break;
    }
}

void SeismicDataSelector::previous()
{
    int order1=ui->cbOrder1->currentIndex();

    switch( order1 ){
    case ILINE_ASC_INDEX: ui->sbInline->stepDown();break;
    case ILINE_DESC_INDEX: ui->sbInline->stepUp(); break;
    case XLINE_ASC_INDEX: ui->sbXline->stepDown(); break;
    case XLINE_DESC_INDEX: ui->sbXline->stepUp(); break;
    default: //nop
        break;
    }
}

void SeismicDataSelector::next()
{
    int order1=ui->cbOrder1->currentIndex();

    switch( order1 ){
    case ILINE_ASC_INDEX: ui->sbInline->stepUp();break;
    case ILINE_DESC_INDEX: ui->sbInline->stepDown(); break;
    case XLINE_ASC_INDEX: ui->sbXline->stepUp(); break;
    case XLINE_DESC_INDEX: ui->sbXline->stepDown(); break;
    default: //nop
        break;
    }
}

void SeismicDataSelector::last()
{
    int order1=ui->cbOrder1->currentIndex();

    switch( order1 ){
    case ILINE_ASC_INDEX: ui->sbInline->setValue(ui->sbInline->maximum()); break;
    case ILINE_DESC_INDEX: ui->sbInline->setValue(ui->sbInline->minimum()); break;
    case XLINE_ASC_INDEX: ui->sbXline->setValue(ui->sbXline->maximum()); break;
    case XLINE_DESC_INDEX: ui->sbXline->setValue(ui->sbXline->minimum()); break;
    default: //nop
        break;
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

void SeismicDataSelector::setLock(bool on){

    if( on==m_lock) return;

    m_lock=on;

    emit lockChanged(on);

    if( !m_lock ) apply();
}

void SeismicDataSelector::setPrimarySort(GatherSortKey key){

    if( key==m_primarySortKey ) return;

    m_primarySortKey=key;

    updateNavigationButtonsEnabled(key);

    emit primarySortChanged(key);
}

void SeismicDataSelector::updateNavigationButtonsEnabled(GatherSortKey key){

    bool useButtons =  ( key==GatherSortKey::Inline || key==GatherSortKey::Crossline );
    ui->pbFirst->setEnabled(useButtons);
    ui->pbPrevious->setEnabled(useButtons);
    ui->pbNext->setEnabled(useButtons);
    ui->pbLast->setEnabled(useButtons);
}


void SeismicDataSelector::apply(){

    if( m_lock ) return;

    readGather();
}


void SeismicDataSelector::setReader(std::shared_ptr<SeismicDatasetReader> reader){
    m_reader=reader;
}

void SeismicDataSelector::setInlineRange( int min, int max){

    if( ui->tbLock->isChecked() ) return;

    ui->sbInline->setRange(min,max);
}

void SeismicDataSelector::setCrosslineRange( int min, int max){

    if( ui->tbLock->isChecked() ) return;

    ui->sbXline->setRange(min,max);
}

void SeismicDataSelector::setInlineCountRange( int min, int max ){

    if( ui->tbLock->isChecked() ) return;

    ui->sbIlineCount->setRange(min, max);
}

void SeismicDataSelector::setCrosslineCountRange( int min, int max ){

    if( ui->tbLock->isChecked() ) return;

    ui->sbXlineCount->setRange(min, max);
}

void SeismicDataSelector::setInline( int val ){

    if( ui->tbLock->isChecked() ) return;

    ui->sbInline->setValue(val);
}

void SeismicDataSelector::setCrossline( int val ){

    if( ui->tbLock->isChecked() ) return;

    ui->sbXline->setValue(val);
}

void SeismicDataSelector::setInlineCount( int count ){

    if( ui->tbLock->isChecked() ) return;

    ui->sbIlineCount->setValue(count);
}

void SeismicDataSelector::setCrosslineCount( int count ){

    if( ui->tbLock->isChecked() ) return;

    ui->sbXlineCount->setValue(count);
}

void SeismicDataSelector::setOrder(int k1, int k2, int k3){

    if( ui->tbLock->isChecked() ) return;

    ui->cbOrder1->setCurrentIndex(k1);
    ui->cbOrder2->setCurrentIndex(k2);
    ui->cbOrder3->setCurrentIndex(k3);
}

// smart version
void SeismicDataSelector::providePoint(int iline, int xline){

   if( ui->tbLock->isChecked() ) return;

   bool isPostStack = m_reader->info().mode() == SeismicDatasetInfo::Mode::Poststack;

   if( isPostStack && (primarySort()==GatherSortKey::Inline) ){
        ui->sbInline->setValue(iline);
   }
   else if( isPostStack && (primarySort()==GatherSortKey::Crossline) ){
        ui->sbXline->setValue(xline);
   }
   else{        // this is also for gathers
       setLock(true);
       ui->sbInline->setValue(iline);
       ui->sbXline->setValue(xline);
       setLock(false);  // calls apply
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
std::cout<<"SeismicDataSelector::provideRandomLine"<<std::endl;
//std::cout<<"#points="<<polyline.size()<<std::endl;
    if( !m_reader ) return;

    if( ui->tbLock->isChecked() ) return;

    if( !ui->tbRandomLine->isChecked()) return;

    // return emptx gather if 0 or 1 point since this is not a line
    if( polyline.size()<2){
        m_gather=std::shared_ptr<seismic::Gather>( new seismic::Gather);
        emit gatherChanged(m_gather);
        return;
    }

    // find all cdp/points on line
    QVector<QPoint> allPoints;
    for( int i=1; i<polyline.size(); i++){

        QPoint p1=polyline[i-1];
        QPoint p2=polyline[i];
        int dx=p2.x() - p1.x();
        int dy=p2.y() - p1.y();
        std::cout<<"dx="<<dx<<" dy="<<dy<<std::endl;
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
    QVector<std::pair<QString,QString>> values;
    for( QPoint point : allPoints){
        std::cout<<"il="<<point.x()<<" xl="<<point.y()<<std::endl;
        values.append(std::make_pair(QString::number(point.y()), QString::number(point.x())));
    }
    m_gather=m_reader->readGather("iline","xline",values);
    /*
    std::shared_ptr<seismic::Gather> gather( new seismic::Gather);
    for( QPoint point : allPoints){
std::cout<<"adding trace il="<<point.x()<<" xl="<<point.y()<<std::endl;
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
*/
    emit gatherChanged(m_gather);
}

void SeismicDataSelector::indexToKey( int idx, QString& key, bool& ascending){

    switch(idx){

    case ILINE_ASC_INDEX:
                ascending=true;
                key="iline";
                break;
    case ILINE_DESC_INDEX:
                ascending=false;
                key="iline";
                break;
    case XLINE_ASC_INDEX:
                ascending=true;
                key="xline";
                break;
    case XLINE_DESC_INDEX:
                ascending=false;
                key="xline";
                break;
    case OFFSET_ASC_INDEX:
                ascending=true;
                key="offset";
                break;
    case OFFSET_DESC_INDEX:
                ascending=false;
                key="offset";
                break;

    default:
        key="";
        ascending=false;
        break;
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

    apply();
}

void SeismicDataSelector::on_sbIlineCount_valueChanged(int arg1)
{
    ui->sbInline->setSingleStep(arg1);
}

void SeismicDataSelector::on_sbXlineCount_valueChanged(int arg1)
{
    ui->sbXline->setSingleStep(arg1);
}

void SeismicDataSelector::on_tbRandomLine_toggled(bool checked)
{
    static int saveIndex1;
    static int saveIndex2;
    static int saveIndex3;

    ui->label->setEnabled(!checked);
    ui->label_2->setEnabled(!checked);
    ui->sbInline->setEnabled(!checked);
    ui->sbIlineCount->setEnabled(!checked);
    ui->sbXline->setEnabled(!checked);
    ui->sbXlineCount->setEnabled(!checked);
    ui->cbOrder1->setEnabled(!checked);
    ui->cbOrder2->setEnabled(!checked);
    ui->cbOrder3->setEnabled(!checked);
    ui->pbFirst->setEnabled(!checked);
    ui->pbPrevious->setEnabled(!checked);
    ui->pbNext->setEnabled(!checked);
    ui->pbLast->setEnabled(!checked);
    //ui->tbInline->setEnabled(!checked);      // active since this unlocks random line
    //ui->tbXLine->setEnabled(!checked);        // active since this unlocks random line

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


void SeismicDataSelector::providePerpendicularLine(int iline, int xline){

    // currently only for poststack data!!
    if( !(m_reader->info().mode()==SeismicDatasetInfo::Mode::Poststack) ) return;

    setLock(true);  // avoid multipkle calls to readGather

    int order1=ui->cbOrder1->currentIndex();
    int order2=ui->cbOrder2->currentIndex();

    // if il/xl sorted make xl/il sort, keep directions (ascending, descending)
    if( order1==ILINE_ASC_INDEX || order1==ILINE_DESC_INDEX ){

        if( order2==XLINE_ASC_INDEX  || order2==XLINE_DESC_INDEX ){

            ui->sbInline->setValue(1);
            ui->sbIlineCount->setValue(m_reader->maxInline() - m_reader->minInline() + 1 );
            ui->sbXline->setValue(xline);
            ui->sbXlineCount->setValue(1);
            ui->cbOrder1->setCurrentIndex(order2);
            ui->cbOrder2->setCurrentIndex(order1);
        }

    }
    // if xl/il sorted make il/xl sort, keep directions (ascending, descending)
    else if( order1==XLINE_ASC_INDEX || order1==XLINE_DESC_INDEX ){

        if( order2==ILINE_ASC_INDEX  || order2==ILINE_DESC_INDEX ){

            ui->sbXline->setValue(1);
            ui->sbXlineCount->setValue(m_reader->maxCrossline() - m_reader->minCrossline() + 1 );
            ui->sbInline->setValue(iline);
            ui->sbIlineCount->setValue(1);
            ui->cbOrder1->setCurrentIndex(order2);
            ui->cbOrder2->setCurrentIndex(order1);
        }

    }

    setLock(false); // calls apply

}

void SeismicDataSelector::on_tbInline_clicked()
{
    setLock(true);

    ui->tbRandomLine->setChecked(false);

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

    setLock(false); // calls apply
}

void SeismicDataSelector::on_tbXLine_clicked()
{   
    setLock(true);

    ui->tbRandomLine->setChecked(false);

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

    setLock(false); // calls apply
}

void SeismicDataSelector::on_tbLock_toggled(bool checked)
{
    ui->cbOrder1->setEnabled(!checked);
    ui->cbOrder2->setEnabled(!checked);
    ui->cbOrder3->setEnabled(!checked);
    ui->sbInline->setEnabled(!checked);
    ui->sbIlineCount->setEnabled(!checked);
    ui->sbXline->setEnabled(!checked);
    ui->sbXlineCount->setEnabled(!checked);
    ui->tbRandomLine->setEnabled(!checked);
    ui->tbInline->setEnabled(!checked);
    ui->tbXLine->setEnabled(!checked);
}

void SeismicDataSelector::on_pbFirst_clicked()
{
    first();
}

void SeismicDataSelector::on_pbPrevious_clicked()
{
    previous();
}

void SeismicDataSelector::on_pbNext_clicked()
{
    next();
}

void SeismicDataSelector::on_pbLast_clicked()
{
    last();
}
