#include "logeditor.h"
#include "ui_logeditor.h"


#include<histogram.h>
#include<histogramdialog.h>

#include <QInputDialog>
#include <QMessageBox>

#include <algorithm>
#include <functional>

#include <multiinputdialog.h>
#include <convertlogdepthtimedialog.h>


struct SaveDivision{
    SaveDivision(double null_value):NULL_VALUE(null_value){
    }

    double operator()(double x, double y){

        return (y!=0) ? x/y : NULL_VALUE;
    }

    double NULL_VALUE;
};

LogEditor::LogEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LogEditor)
{
    ui->setupUi(this);

    setAttribute( Qt::WA_DeleteOnClose);

    ui->trackView->setCursorMode(AxisView::CURSORMODE::BOTH);
    ui->trackView->setZoomMode(AxisView::ZOOMMODE::BOTH);
    ui->trackView->setXMesh(true);
    ui->trackView->setZMesh(true);

    // receive mouse position
    connect( ui->trackView->xAxis(), SIGNAL(cursorPositionChanged(qreal)), this, SLOT(onXCursorChanged(qreal)) );
    connect( ui->trackView->zAxis(), SIGNAL(cursorPositionChanged(qreal)), this, SLOT(onZCursorChanged(qreal)) );

    ui->trackView->setSelectionMode(AxisView::SELECTIONMODE::Polygon);
}

LogEditor::~LogEditor()
{
    delete ui;
}

void LogEditor::setProject( AVOProject* p){

    if( p==m_project ) return;

    m_project=p;
}


void LogEditor::setUWI(const QString & s){

    if( !m_project->existsWell(s)) return;

    m_uwi=s;
    auto info=m_project->getWellInfo(s);
    setWindowTitle(QString("%1 - %2").arg(s,info.name()));
}

void LogEditor::setLog(std::shared_ptr<Log> log){

    ui->trackLabel->setText( QString("%1 [%2] ").arg(log->name().c_str(), log->unit().c_str() ) ); // update name and unit anyway, they could have changed

    if( log == m_log ) return;

    m_log=log;
/*
    ui->trackView->zAxis()->setRange(log->z0(), log->lz() );
    ui->trackView->zAxis()->setViewRange(log->z0(), log->lz());

    // this in done in trackview::setLog
    //auto range = log->range();
    //ui->trackView->xAxis()->setRange(range.first, range.second);
    //ui->trackView->xAxis()->setViewRange(range.first, range.second);


*/
    ui->trackView->setLog(log);
    auto range=m_log->range();
    ui->trackView->xAxis()->setRange(range.first, range.second);
    ui->trackView->xAxis()->setViewRange(range.first, range.second);
    ui->trackView->xAxis()->adjustRange();

    ui->trackView->zAxis()->setRange(m_log->z0(), m_log->lz());
    ui->trackView->zAxis()->setViewRange(m_log->z0(), m_log->lz());
}


void LogEditor::on_action_Open_Log_triggered()
{
    if( !askModifiedProceed(tr("Open Log"))) return;

    openLog();
}

void LogEditor::on_action_Save_Log_As_triggered()
{
    const int MAX_VERSIONS=100;

    if( !m_log) return;

    QString prev_name;

    for(int i=1; i<MAX_VERSIONS; i++){
        prev_name = QString("%1 - %2").arg(m_log->name().c_str(), QString::number(i));
        if( !m_project->existsLog(m_uwi, prev_name) ) break;
    }

    for(;;){
        bool ok=false;
        QString name=QInputDialog::getText(this, tr("Save Log"), tr("Enter Name:"), QLineEdit::Normal, prev_name, &ok );
        if( name.isNull() || !ok ) break;

        if( m_project->existsLog(m_uwi, name ) ){
            QMessageBox::information(this, tr("Save Log"), QString("Well \"%1\" already has a log named \"%2\"").arg(m_uwi, name));
            prev_name =name;    // retry
        }

        if( !m_project->addLog(m_uwi, name, *m_log) ){
            QMessageBox::critical(this, tr("Save Log"), tr("Adding log to project failed!") );
            return;
        }

        setDirty(false);

        break;  // saved it
    }
}

void LogEditor::on_actionClip_triggered()
{
    if( !m_log ) return;

    auto range = m_log->range();

    bool ok=false;

    /*
    double min = QInputDialog::getDouble(this, tr("Clip Log"), tr("Enter Minimum Value:"), range.first, range.first, range.second, 1, &ok);
    if( !ok ) return;

    double max = QInputDialog::getDouble(this, tr("Clip Log"), tr("Enter Maximum Value:"), range.second, min, range.second, 1, &ok);
    if( !ok ) return;
    */
    auto minmax = MultiInputDialog::getDoubles(this, tr("Clip Log"), QStringList{"Minimum:", "Maximum:"});
    if( minmax.size()<2) return;
    double min=minmax[0];
    double max=minmax[1];

    auto res=std::make_shared<Log>( "clipped", m_log->unit(), "clipped", m_log->z0(), m_log->dz(), m_log->nz() );

    for( int i=0; i<m_log->nz(); i++){

        auto x = (*m_log)[i];

        if( x==m_log->NULL_VALUE){
            (*res)[i]=res->NULL_VALUE;
        }
        else if(x<min){
            (*res)[i]=min;
        }
        else if(x>max){
            (*res)[i]=max;
        }
        else{
            (*res)[i]=x;
        }
    }

    setLog(res);

    setDirty(true);
}

void LogEditor::on_action_Median_triggered()
{
    if( !m_log ) return;

    auto res=std::make_shared<Log>( "median filtered", m_log->unit(), "median filtered", m_log->z0(), m_log->dz(), m_log->nz() );

    bool ok=false;
    int len=QInputDialog::getInt(this, "Median Filter", "Filter length [samples]:", 100, 0, m_log->nz(), 10, &ok);

    if( !ok ) return;



    for( int i=0; i<m_log->nz(); i++ ){

        int start = std::max( 0, i-len/2);
        int stop = std::min(m_log->nz()-1, i + len/2 );
        std::vector<double> buf;
        buf.reserve(len);
        for( int i=start; i<=stop; i++){
            if( (*m_log)[i]==m_log->NULL_VALUE) continue;
            buf.push_back((*m_log)[i]);
        }

        if( buf.size()>0 ){
            std::nth_element( &buf[0], &buf[buf.size()/2], &buf[buf.size()-1] );
            (*res)[i]=buf[buf.size()/2];
        }
        else{
            (*res)[i]=res->NULL_VALUE;
        }
    }

    setLog(res);

    setDirty(true);
}

void LogEditor::on_actionMean_Filter_triggered()
{
    if( !m_log ) return;

    auto res=std::make_shared<Log>( "mean filtered", m_log->unit(), "median filtered", m_log->z0(), m_log->dz(), m_log->nz() );

    bool ok=false;
    int len=QInputDialog::getInt(this, "Mean Filter", "Filter length [samples]:", 100, 0, m_log->nz(), 10, &ok);

    if( !ok ) return;

    for( int i=0; i<m_log->nz(); i++ ){

        int start = std::max( 0, i-len/2);
        int stop = std::min(m_log->nz()-1, i + len/2 );
        // dont use accumulate because need to handle null values
        double sum=0;
        int n=0;
        for( int i=start; i<=stop; i++){
            if( (*m_log)[i]==m_log->NULL_VALUE ) continue;
            sum+=(*m_log)[i];
            n++;
        }

        (*res)[i] = (n>0) ? sum/n : res->NULL_VALUE;
    }

    setLog(res);

    setDirty(true);
}



void LogEditor::closeEvent( QCloseEvent* event ){

    if( askModifiedProceed(tr("Close Editor")) ){
        event->accept();
    }
    else{
        event->ignore();
    }
}

void LogEditor::setDirty(bool on){
    m_dirty=on;
}

bool LogEditor::askModifiedProceed( QString title ){

    if( !isDirty() ) return true;

    auto reply =QMessageBox::warning(this, title, tr("Unsaved changes, Proceed?"), QMessageBox::Yes | QMessageBox::No );

    return (reply==QMessageBox::Yes);
}

void LogEditor::on_actionHistogram_triggered()
{
    QVector<double> data;
    data.reserve(m_log->nz());
    for(  auto x : *m_log ){
        if( x==m_log->NULL_VALUE) continue;
        data.push_back(x);
    }

    if(data.empty()) return;

    HistogramDialog* viewer=new HistogramDialog;
    viewer->setData( data );
    viewer->setWindowTitle(QString("Histogram of %1 - %2").arg(m_uwi, m_log->name().c_str() ) );

    viewer->show();
}

class RunningAverage{
public:
    RunningAverage(){
    }
    double operator()(double x){
        n++;
        sum+=x;
        return sum/n;
    }
private:
    double sum=0;
    size_t n=0;
};

void LogEditor::on_actionRunning_Average_triggered()
{
    RunningAverage ravg;
    logOp( tr("Running Average"), tr(""), ravg );
}


void LogEditor::on_action_Invert_triggered()
{
    logOp( tr("Invert Log"), tr("^-1"), [](double x){return 1./x;});
}

void LogEditor::on_actionAdd_Constant_triggered()
{
    logValueOp( tr("Add Value"), tr("+"), [](double x, double y){return x+y;});
}

void LogEditor::on_actionMultiply_Constant_triggered()
{
    logValueOp( tr("Multiply Value"), tr("*"), [](double x, double y){return x*y;});
}


void LogEditor::on_actionAdd_Log_triggered()
{
    logLogOp( tr("Add Log"), tr("+"), [](double x, double y){return x+y;});
}

void LogEditor::on_actionSubtract_Log_triggered()
{
    logLogOp( tr("Subtract Log"), tr("-"), [](double x, double y){return x-y;});
}

void LogEditor::on_actionMultiply_Log_triggered()
{
    logLogOp( tr("Multiply Log"), tr("*"), [](double x, double y){return x*y;});
}

void LogEditor::on_actionDivide_Log_triggered()
{
    SaveDivision op(m_log->NULL_VALUE);
    logLogOp( tr("Divide Log"), tr("/"), op);//[](double x, double y){return x/y;});
}



void LogEditor::on_actionEdit_Name_triggered()
{
    bool ok=false;
    QString name = QInputDialog::getText(this, tr("Edit Name"), tr("Log Name:"), QLineEdit::Normal, m_log->name().c_str(), &ok);

    if( !ok || name.isNull()) return;

    m_log->setName(name.toStdString());

    setLog(m_log);
    setDirty(true);
}

void LogEditor::on_actionEdit_Unit_triggered()
{
    bool ok=false;
    QString unit = QInputDialog::getText(this, tr("Edit Unit"), tr("Log Unit:"), QLineEdit::Normal, m_log->unit().c_str(), &ok);

    if( !ok || unit.isNull()) return;

    m_log->setUnit(unit.toStdString());

    setLog(m_log);
    setDirty(true);
}

void LogEditor::on_actionEdit_Description_triggered()
{
    bool ok=false;
    QString descr = QInputDialog::getMultiLineText( this, tr("Edit Description"), tr("Log Description:"), m_log->descr().c_str(), &ok);

    if( !ok || descr.isNull()) return;

    m_log->setDescr(descr.toStdString());

    setLog(m_log);
    setDirty(true);
}


void LogEditor::on_actionEdit_Sampling_triggered()
{
    bool ok=false;
    auto res=MultiInputDialog::getDoubles(this, tr("Log Sampling Parameters"), QStringList{"Start:", "Interval:"},
                                          QVector<double>{m_log->z0(), m_log->dz()}, &ok);

    if( !ok || res.size()<2) return;

    auto log = std::make_shared<Log>(*m_log);
    log->setZ0(res[0]);
    log->setDZ(res[1]);

    setLog(log);

    setDirty(true);
}


void LogEditor::onXCursorChanged(qreal x){
    QString msg=QString("x=%1, z=%2").arg(x).arg(ui->trackView->xAxis()->cursorPosition());
    statusBar()->showMessage(msg);
 }

void LogEditor::onZCursorChanged(qreal z){

    if( !m_log ) return;

    int j = (z-m_log->z0())/m_log->dz();
    QString text;
    if( j>=0 && j<m_log->nz() ){
        auto value = (*m_log)[j];
        if( value!=m_log->NULL_VALUE){
            text=QString("%1 %2").arg(value).arg(m_log->unit().c_str());
        }
    }
    ui->trackValueLabel->setText(text);

    QString msg=QString("x=%1, z=%2").arg(ui->trackView->xAxis()->cursorPosition()).arg(z);
    statusBar()->showMessage(msg);
}


void LogEditor::openLog(){

    bool ok=false;
    QString name = QInputDialog::getItem(this, tr("Open Log"), tr("Select Log:"), m_project->logList(m_uwi), 0, false, &ok );

    if( name.isEmpty() || !ok ) return;

    openLog(name);
}


void LogEditor::openLog(QString name){

    auto log=m_project->loadLog(m_uwi, name);
        if( !log ){
        QMessageBox::critical(this, tr("Open Log"), QString("Loading log \"%1 - %2\" failed!").arg(m_uwi).arg(name));
        return;
    }

    setLog(log);

    setDirty(false);
}

bool LogEditor::logOp(QString opName, QString opAbbr, std::function<double(double)> op )
{
    auto res = std::make_shared<Log>( QString("%1-edit").arg(m_log->name().c_str()).toStdString() ,
                                      QString("%1%2").arg(m_log->unit().c_str(), opAbbr).toStdString(),
                                      QString("%1%2").arg(m_log->name().c_str(), opAbbr).toStdString() ,
                                      m_log->z0(), m_log->dz(), m_log->nz());
    if( !res){
        QMessageBox::critical(this, opName, tr("Allocating result failed!"));
        return false;
    }

    for( int i=0; i<m_log->nz(); i++){
        auto x = (*m_log)[i];
        if( x==m_log->NULL_VALUE ){
            (*res)[i]=res->NULL_VALUE;
        }
        else{
            (*res)[i]=op(x);
        }
    }

    setLog(res);
    setDirty(true);
    return true;
}


bool LogEditor::logLogOp(QString opName, QString opAbbr, std::function<double(double, double)> op )
{
    bool ok=false;
    QString name = QInputDialog::getItem(this, opName, tr("Select Log"), m_project->logList(m_uwi), 0, false, &ok);
    if( !ok || name.isNull()) return false;

    auto other=m_project->loadLog(m_uwi, name);
    if( !other ){
        QMessageBox::critical(this, tr("Add Log"), QString("Loading log \"%1\" failed!").arg(name));
        return false;
    }

    if( other->nz()!=m_log->nz()){
        QMessageBox::warning(this, tr("Add Log"), tr("Logs have different number of samples!"));
        return false;
    }


    auto res = std::make_shared<Log>( QString("%1-edit").arg(m_log->name().c_str()).toStdString() ,
                                      QString("%1%2%3").arg(m_log->unit().c_str(), opAbbr, other->unit().c_str()).toStdString(),
                                      QString("%1%2%3").arg(m_log->name().c_str(), opAbbr, other->name().c_str()).toStdString() ,
                                      m_log->z0(), m_log->dz(), m_log->nz());
    if( !res){
        QMessageBox::critical(this, opName, tr("Allocating result failed!"));
        return false;
    }

    for( int i=0; i<m_log->nz(); i++){
        auto x = (*m_log)[i];
        auto y = (*other)[i];
        if( x==m_log->NULL_VALUE || y==other->NULL_VALUE){
            (*res)[i]=res->NULL_VALUE;
        }
        else{
            (*res)[i]=op(x,y);
        }
    }

    setLog(res);
    setDirty(true);
    return true;
}

bool LogEditor::logValueOp(QString opName, QString opAbbr, std::function<double(double, double)> op )
{
    bool ok=false;
    double value = QInputDialog::getDouble(this, opName, tr("Value:"), 0,
                                           std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), 1, &ok );
    if( !ok ) return false;

    auto res = std::make_shared<Log>( QString("%1-edit").arg(m_log->name().c_str()).toStdString() ,
                                      m_log->unit().c_str(),
                                      QString("%1%2%3").arg(m_log->name().c_str(), opAbbr).arg(value).toStdString() ,
                                      m_log->z0(), m_log->dz(), m_log->nz());

    if( !res){
        QMessageBox::critical(this, opName, tr("Allocating result failed!"));
        return false;
    }

    for( int i=0; i<m_log->nz(); i++){
        auto x = (*m_log)[i];
        if( x==m_log->NULL_VALUE){
            (*res)[i]=res->NULL_VALUE;
        }
        else{
            (*res)[i]=op(x,value);
        }
    }

    setLog(res);
    setDirty(true);
    return true;
}

// check argv has at least as many samples as log
std::shared_ptr<Log> convertLogDepthTime( const Log& log, const Log& avgv, double ft, double dt, int nt){    // times in secs

    // convert log depths to time
    std::vector<double> z;
    std::vector<double> t;
    z.reserve(log.nz());
    t.reserve(log.nz());
    for(int i=0; i<log.nz(); i++){
        auto vi=avgv[i];
        auto zi=i*log.dz(); // time zero at z=0!!!   log.index2z(i);
        auto ti=zi / vi;
        z.push_back(zi);
        t.push_back(ti);
    }

    auto res=std::make_shared<Log>( log.name()+"-dtc", log.unit(), "Depth to Time converted", ft, dt, nt, 0);

    for( int i=0; i<nt; i++){

        auto ti =res->index2z(i);    // output sample time
        // find input log sample before or equal
        int j=0;
        while( j+1<t.size() && t[j+1]<ti ) j++;     // need to replace this with bisection
        if( j+1>=t.size()) continue;

        // linear interpolation of output sample
        auto val0=log[j];
        auto val1=log[j+1];
        if( val0==log.NULL_VALUE || val1==log.NULL_VALUE) continue; // skip nulls

        auto t0=t[j];
        auto t1=t[j+1];
        auto w1 = (ti-t0)/(t1-t0);
        auto vali=(1.-w1)*val0 + w1*val1;
        std::cout<<"i="<<i<<" ti="<<ti<<" j="<<j<<" t0="<<t0<<" t1="<<t1<<" w1="<<w1<<" vali="<<vali<<std::endl<<std::flush;
        (*res)[i]=vali;
    }

    return res;
}

void LogEditor::on_actionDepth_to_Time_triggered()
{

    ConvertLogDepthTimeDialog dlg;
    dlg.setVelocityLogs(m_project->logList(m_uwi));
    dlg.setWindowTitle(tr("Convert Log Depth to Time"));
    if( dlg.exec()!=QDialog::Accepted) return;

    auto avgv_name=dlg.velocityLog();
    auto avgv=m_project->loadLog(m_uwi, avgv_name );
    if( !avgv ){
        QMessageBox::critical(this, tr("Convert Log Depth to Time"), QString("Loading log \"%1 - %2\" failed!").arg(m_uwi).arg(avgv_name), QMessageBox::Ok);
        return;
    }

    /* Ignore path for now
    auto path=m_project->loadWellPath(m_uwi);
    if( !path ){
        QMessageBox::critical( this, tr("Convert Log Depth to Time"), tr("Loading Wellpath failed!"), QMessageBox::Ok);
        return;
    }
    */

    auto res=convertLogDepthTime( *m_log, *avgv, 0.001*dlg.start(), 0.001*dlg.interval(), (dlg.stop()-dlg.start())/dlg.interval());

    setLog(res);
    setDirty(true);
}
