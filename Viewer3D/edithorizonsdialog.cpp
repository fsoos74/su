#include "edithorizonsdialog.h"
#include "ui_edithorizonsdialog.h"
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include<iostream>

EditHorizonsDialog::EditHorizonsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditHorizonsDialog)
{
    ui->setupUi(this);

    connect( ui->cbColor, SIGNAL(colorChanged(QColor)), this, SLOT(horizonControlsChanged()) );
    connect( ui->cbUseColor, SIGNAL(toggled(bool)), this, SLOT(horizonControlsChanged()) );
    connect( ui->sbDelay, SIGNAL(valueChanged(int)), this, SLOT(horizonControlsChanged()) );
}

EditHorizonsDialog::~EditHorizonsDialog()
{
    delete ui;
}


void EditHorizonsDialog::setProject(std::shared_ptr<AVOProject> project){

    m_project=project;
}


void EditHorizonsDialog::setHorizonModel(HorizonModel * model){

    if( model == m_horizonModel ) return;

    m_horizonModel=model;

    ui->cbHorizon->clear();

    if( !m_horizonModel )  return;

    connect( m_horizonModel, SIGNAL(changed()), this, SLOT(modelChanged()));

    foreach( QString name, m_horizonModel->names()){
        ui->cbHorizon->addItem(name);
    }

    ui->cbHorizon->setCurrentIndex(0);
}


void EditHorizonsDialog::setCurrentHorizon(QString name){

    if( !m_horizonModel || !m_horizonModel->contains(name)) return;

    // this will trigger update of controls via currentIndexChanged
    ui->cbHorizon->setCurrentText(name);
}


void EditHorizonsDialog::modelChanged(){

    if( !m_horizonModel ) return;

    QString name = ui->cbHorizon->currentText();
    HorizonDef current=horizonFromControls(name);

    wait_controls=true;

    ui->cbHorizon->clear();
    QStringList names;
    foreach( QString s, m_horizonModel->names()){
        names.append(s);
    }
    ui->cbHorizon->addItems(names);

    wait_controls=false;

    // if exists make same horizon current as before
    if( names.contains(name)){
        ui->cbHorizon->setCurrentText(name);
    }
    else{
        ui->cbHorizon->setCurrentIndex(0);
    }
}


void EditHorizonsDialog::horizonControlsChanged(){

    // update of controls in progress, dont update until all controls are set to the new values
    if( wait_controls) return;

    QString name=ui->cbHorizon->currentText();
    HorizonDef horizon=horizonFromControls(name);

    // keep grid, no need to store it elsewhere
    if( m_horizonModel->contains(name)){
        horizon.horizon = m_horizonModel->item(name).horizon;
    }

    m_horizonModel->setItem(name, horizon);
}

void EditHorizonsDialog::horizonToControls( QString name, HorizonDef horizon ){


    // prevent signals of controls before all params are set
    wait_controls=true;

    // update value range according to slice type and volume bounds

    m_currentHorizonControl = horizon.horizon;  // keep

    ui->cbUseColor->setChecked(horizon.useColor);
    ui->cbColor->setColor(horizon.color);
    ui->sbDelay->setValue(horizon.delay);

    // value changes of controls can trigger signals again
    wait_controls=false;
}

HorizonDef EditHorizonsDialog::horizonFromControls( QString name){

    HorizonDef def;

    def.horizon=m_currentHorizonControl;
    def.useColor=ui->cbUseColor->isChecked();
    def.color=ui->cbColor->color();
    def.delay=ui->sbDelay->value();

    return def;
}



void EditHorizonsDialog::on_pbRemove_clicked()
{

    QString name = ui->cbHorizon->currentText();
    m_horizonModel->removeItem(name);
}

void EditHorizonsDialog::on_cbHorizon_currentIndexChanged(const QString &name)
{

    if(wait_controls) return;

    Q_ASSERT( m_horizonModel->contains(name));

    HorizonDef def=m_horizonModel->item(name);

    horizonToControls(name, def);
}

void EditHorizonsDialog::on_pbAdd_clicked()
{
    static const QVector<QColor> HorizonColors{Qt::darkRed, Qt::darkGreen, Qt::darkBlue,
                Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow,
                Qt::red, Qt::green, Qt::blue,
                Qt::cyan, Qt::magenta, Qt::yellow};


    if( !m_project ) return;

    QStringList notLoaded;
    foreach( QString name, m_project->gridList(GridType::Horizon) ){

        if( ! m_horizonModel->contains( name )) notLoaded.append(name);
    }

    // nothing left to load
    if( notLoaded.isEmpty() ) return;

    bool ok=false;
    QString gridName=QInputDialog::getItem(this, "Open Horizon", "Please select a horizon:",
                                           notLoaded, 0, false, &ok);
    if( !gridName.isEmpty() && ok ){

        std::shared_ptr<Grid2D<float> > grid=m_project->loadGrid(GridType::Horizon, gridName );
        if( !grid ){
            QMessageBox::critical(this, tr("Add Horizon"), QString("Loading horizon \"%1\" failed!"). arg(gridName) );
            return;
        }

        HorizonDef def{ grid, false, HorizonColors.at(m_horizonModel->size()%HorizonColors.size()), 0 };

        m_horizonModel->addItem(gridName, def);

        setCurrentHorizon(gridName);
    }
}

void EditHorizonsDialog::on_cbColor_clicked()
{
    QColor color=QColorDialog::getColor( ui->cbColor->color(), this, tr("Select a Color"));

    if( color.isValid()){
        ui->cbColor->setColor(color);
    }
}
