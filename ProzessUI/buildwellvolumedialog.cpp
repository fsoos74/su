#include "buildwellvolumedialog.h"
#include "ui_buildwellvolumedialog.h"

#include<QIntValidator>
#include<QDoubleValidator>
#include<QPushButton>
#include <wellpath.h>
#include<cmath>

BuildWellVolumeDialog::BuildWellVolumeDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::BuildWellVolumeDialog)
{
    ui->setupUi(this);

    QIntValidator* ivalidator=new QIntValidator(this);
    ui->leMinIline->setValidator(ivalidator);
    ui->leMaxIline->setValidator(ivalidator);
    ui->leMinXline->setValidator(ivalidator);
    ui->leMaxXline->setValidator(ivalidator);
    ui->leMinZ->setValidator(ivalidator);
    ui->leMaxZ->setValidator(ivalidator);
    ui->cbDZ->setValidator(ivalidator);

    connect(ui->leMinIline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxIline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMinXline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxXline, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );

    connect(ui->leMinIline, SIGNAL(textChanged(QString)), this, SLOT(updateLogs()) );
    connect(ui->leMaxIline, SIGNAL(textChanged(QString)), this, SLOT(updateLogs()) );
    connect(ui->leMinXline, SIGNAL(textChanged(QString)), this, SLOT(updateLogs()) );
    connect(ui->leMaxXline, SIGNAL(textChanged(QString)), this, SLOT(updateLogs()) );

    connect(ui->leMinZ, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->leMaxZ, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect(ui->cbDZ, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->leVolume, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbLog, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateWells(QString)) );
    connect( ui->cbRestrictToArea, SIGNAL(toggled(bool)), this, SLOT(updateLogs()) );

    connect( ui->lwWells, SIGNAL(itemSelectionChanged()), this, SLOT(updateOkButton()) );

    updateOkButton();
}

BuildWellVolumeDialog::~BuildWellVolumeDialog()
{
    delete ui;
}


void BuildWellVolumeDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    if( m_project){
        auto geom=m_project->geometry();
        ui->leMinIline->setText(QString::number(geom.bboxLines().left()));
        ui->leMaxIline->setText(QString::number(geom.bboxLines().right()));
        ui->leMinXline->setText(QString::number(geom.bboxLines().top()));
        ui->leMaxXline->setText(QString::number(geom.bboxLines().bottom()));
    }

    updateLogs();
    updateHorizons();
}

QMap<QString, QString> BuildWellVolumeDialog::params(){

    QMap<QString, QString> p;

    p.insert( QString("volume"), ui->leVolume->text());
    p.insert( QString("min-iline"), ui->leMinIline->text());
    p.insert( QString("max-iline"), ui->leMaxIline->text());
    p.insert( QString("min-xline"), ui->leMinXline->text());
    p.insert( QString("max-xline"), ui->leMaxXline->text());
    p.insert( QString("min-z"), ui->leMinZ->text());
    p.insert( QString("max-z"), ui->leMaxZ->text());
    p.insert( QString("dz"), ui->cbDZ->currentText());

    p.insert( QString("filter-len"), QString::number(ui->sbFilter->value()));

    p.insert( QString("top-horizon"), ui->cbTopHorizon->currentText() );
    p.insert( QString("bottom-horizon"), ui->cbBottomHorizon->currentText() );

    p.insert( QString("log"), ui->cbLog->currentText() );

    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    int i=0;
    for( auto mi : ids ){
        QString well=ui->lwWells->item(mi.row())->text();
        p.insert( QString("well%1").arg(++i), well );
    }

    return p;
}


void BuildWellVolumeDialog::updateHorizons(){

    QStringList items;
    items<<"NONE";
    items<<m_project->gridList(GridType::Horizon);
    ui->cbTopHorizon->clear();
    ui->cbTopHorizon->addItems(items);
    ui->cbBottomHorizon->clear();
    ui->cbBottomHorizon->addItems(items);
}


QStringList BuildWellVolumeDialog::wellList(){

    if( !ui->cbRestrictToArea->isChecked()) return m_project->wellList();

    QStringList wells;

    auto il1=ui->leMinIline->text().toInt();
    auto il2=ui->leMaxIline->text().toInt();
    auto xl1=ui->leMinXline->text().toInt();
    auto xl2=ui->leMaxXline->text().toInt();
    QTransform xy_to_ilxl, ilxl_to_xy;
    m_project->geometry().computeTransforms(xy_to_ilxl, ilxl_to_xy);

    for( auto well : m_project->wellList()){
        auto wp=m_project->loadWellPath(well);
        if( !wp || wp->size()<1) continue;
        auto xyz=(*wp)[0];
        auto ilxl=xy_to_ilxl.map(QPointF(xyz[0], xyz[1]));
        int il=std::round(ilxl.x());
        int xl=std::round(ilxl.y());
        if( il<il1 || il>il2 || xl<xl1 || xl>xl2 ) continue;
        wells<<well;
    }

    return wells;
}

void BuildWellVolumeDialog::updateLogs(){

    ui->cbLog->clear();

    if( !m_project ) return;

    QStringList loglist;

    for( auto well :  wellList() ){//m_project->wellList()){

        for( auto log : m_project->logList(well)){
            if( !loglist.contains(log)){
                loglist.push_back(log);
            }
        }
    }

    std::sort( loglist.begin(), loglist.end());

    ui->cbLog->addItems(loglist);
}


void BuildWellVolumeDialog::updateWells( QString log ){

    ui->lwWells->clear();

    if( !m_project) return;

    QStringList welllist;

    for( auto well : wellList() ){//m_project->wellList() ){

        if( m_project->logList(well).contains(log)){
            welllist.push_back(well);
        }

    }

    ui->lwWells->addItems(welllist);
}


void BuildWellVolumeDialog::updateOkButton(){

    bool ok=true;

    // test volume name
    if( ui->leVolume->text().isEmpty()){
        ok=false;
    }

    {
    QPalette palette;
    if( m_project && m_project->volumeList().contains(ui->leVolume->text())){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leVolume->setPalette(palette);
    }


    // test inline range
    {
    QPalette palette;
    if( ui->leMinIline->text().toInt() >= ui->leMaxIline->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinIline->setPalette(palette);
    ui->leMaxIline->setPalette(palette);
    }

    // test crossline range
    {
    QPalette palette;
    if( ui->leMinXline->text().toInt() >=ui->leMaxXline->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinXline->setPalette(palette);
    ui->leMaxXline->setPalette(palette);
    }

    // test depth/time range
    {
    QPalette palette;
    if( ui->leMinZ->text().toInt() >= ui->leMaxZ->text().toInt() ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->leMinZ->setPalette(palette);
    ui->leMaxZ->setPalette(palette);
    }

    // test sampling interval
    {
    QPalette palette;
    if( ui->cbDZ->currentText().toInt() <= 0 ){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->cbDZ->setPalette(palette);
    }

    // test if any wells are selected
    if( ui->lwWells->selectedItems().isEmpty()) ok=false;

    ui->buttonBox->button( QDialogButtonBox::Ok)->setEnabled(ok);
}

