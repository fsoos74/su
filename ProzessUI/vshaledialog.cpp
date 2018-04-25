#include "vshaledialog.h"
#include "ui_vshaledialog.h"

#include<QComboBox>
#include<QPushButton>
#include<vshaleprocess.h>


VShaleDialog::VShaleDialog(QWidget *parent) :
    ProcessParametersDialog(parent),
    ui(new Ui::VShaleDialog)
{
    ui->setupUi(this);

    ui->cbBlockingMode->addItem(toQString(VShaleProcess::BlockingMode::Curves));
    ui->cbBlockingMode->addItem(toQString(VShaleProcess::BlockingMode::LayersTops));

    connect( ui->lwWells->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateInputLogs()) );
    connect( ui->lwWells->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateTops()) );
    connect( ui->leIGR, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbTertiaryRocks, SIGNAL(toggled(bool)), this, SLOT(updateOkButton()));
    connect( ui->leTertiaryRocks, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbOlderRocks, SIGNAL(toggled(bool)), this, SLOT(updateOkButton()));
    connect( ui->leOlderRocks, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbSteiber, SIGNAL(toggled(bool)), this, SLOT(updateOkButton()));
    connect( ui->leSteiber, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbClavier, SIGNAL(toggled(bool)), this, SLOT(updateOkButton()));
    connect( ui->leClavier, SIGNAL(textChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbGRMin, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->cbGRMax, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOkButton()) );
    connect( ui->lwTops->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(updateOkButton()) );

    updateOkButton();
}

VShaleDialog::~VShaleDialog()
{
    delete ui;
}


QMap<QString,QString> VShaleDialog::params(){

    QMap<QString, QString> p;

    auto wells=selectedWells();
    p.insert("wells", packParamList(wells));
    p.insert( "gr-log", ui->cbGR->currentText() );

    p.insert( "blocking-mode", ui->cbBlockingMode->currentText() );
    p.insert( "grmin-log", ui->cbGRMin->currentText() );
    p.insert( "grmax-log", ui->cbGRMax->currentText() );
    p.insert( "blocking-tops", selectedTops().join(";"));
    p.insert( "igr-log", ui->leIGR->text() );
    p.insert( "vsh-tertiary-rocks-log", (ui->cbTertiaryRocks->isChecked()) ? ui->leTertiaryRocks->text() : "" );
    p.insert( "vsh-older-rocks-log", (ui->cbOlderRocks->isChecked()) ? ui->leOlderRocks->text() : "" );
    p.insert( "vsh-steiber-log", (ui->cbSteiber->isChecked()) ? ui->leSteiber->text() : "" );
    p.insert( "vsh-clavier-log", (ui->cbClavier->isChecked()) ? ui->leClavier->text() : "" );
    return p;
}


void VShaleDialog::setProject(AVOProject* project){

    if( project==m_project) return;

    m_project = project;

    auto wells=m_project->wellList();
    ui->lwWells->clear();
    ui->lwWells->addItems(wells);
}


void VShaleDialog::updateInputLogs(){

    ui->cbGR->clear();
    ui->cbGRMin->clear();
    ui->cbGRMax->clear();
    if( !m_project) return;

    auto wells=selectedWells();
    if( wells.isEmpty()) return;

    QSet<QString> logs=m_project->logList(wells.front()).toSet();
    for( auto well : wells){
        logs.intersect(m_project->logList(well).toSet());
    }

    auto l = logs.toList();
    std::sort(l.begin(), l.end());

    ui->cbGR->addItems(l);
    ui->cbGRMin->addItems(l);
    ui->cbGRMax->addItems(l);
}

void VShaleDialog::updateTops(){


    ui->lwTops->clear();

    if( !m_project) return;

    auto wells=selectedWells();
    if( wells.isEmpty()) return;

    auto pmks=m_project->loadWellMarkersByWell(wells.front());
    if(!pmks) return;
    QSet<QString> tops;
    for( auto wm : *pmks){
        tops.insert(wm.name());
    }
    for( auto well : wells){
        auto pmks=m_project->loadWellMarkersByWell(wells.front());
        if(!pmks) return;
        QSet<QString> wtops;
        for( auto wm : *pmks){
            wtops.insert(wm.name());
        }
        tops.intersect(wtops);
    }

    ui->lwTops->addItems(tops.toList());
}

void VShaleDialog::updateOkButton(){

    if(!m_project) return;

    bool ok=true;
    QStringList onames;

    auto wells=selectedWells();

    QSet<QString> usedLogs;
    for( auto well : selectedWells()){
        usedLogs.unite(m_project->logList(well).toSet());
    }


    switch( toBlockingMode(ui->cbBlockingMode->currentText())){
    case VShaleProcess::BlockingMode::LayersTops:{
        QPalette palette;
        if( selectedTops().size()<2){
            ok=false;
            palette.setColor(QPalette::Text, Qt::red);
        }
        ui->lwTops->setPalette(palette);
        break;
    }
    default:
        break;
    }

    {
    QPalette palette;
    auto igrname=ui->leIGR->text();
    if( igrname.isEmpty() ){
        ok=false;
    }
    else if(usedLogs.contains(igrname)){
        ok=false;
        palette.setColor(QPalette::Text, Qt::red);
    }
    onames<<igrname;
    ui->leIGR->setPalette(palette);
    }


    for( auto cble : { std::make_pair(ui->cbTertiaryRocks, ui->leTertiaryRocks),
                       std::make_pair(ui->cbOlderRocks, ui->leOlderRocks),
                       std::make_pair(ui->cbSteiber, ui->leSteiber),
                       std::make_pair(ui->cbClavier, ui->leClavier)}
         ){
        QPalette palette;
        if( cble.first->isChecked()){
            auto name=cble.second->text();
            if( name.isEmpty() || onames.contains(name) || usedLogs.contains(name)){
                ok=false;
                palette.setColor(QPalette::Text, Qt::red);
            }
            onames<<name;
        }
        cble.second->setPalette(palette);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


void VShaleDialog::on_cbBlockingMode_currentIndexChanged(const QString &s)
{
    ui->cbGRMin->setEnabled(s==toQString(VShaleProcess::BlockingMode::Curves));
    ui->cbGRMax->setEnabled(s==toQString(VShaleProcess::BlockingMode::Curves));
    ui->lwTops->setEnabled(s==toQString(VShaleProcess::BlockingMode::LayersTops));
}

QStringList VShaleDialog::selectedWells(){

    QStringList wells;
    auto ids = ui->lwWells->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        wells<<ui->lwWells->item(idx.row())->text();
    }
    return wells;
}

QStringList VShaleDialog::selectedTops(){

    QStringList tops;
    auto ids = ui->lwTops->selectionModel()->selectedIndexes();
    for( auto idx : ids){
        tops<<ui->lwTops->item(idx.row())->text();
    }
    return tops;
}
