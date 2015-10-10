#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<horizoninputdialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    gridView = ui->gridView;//new GridView(this);

    gridView->setBackgroundRole(QPalette::Dark);
    setCentralWidget(gridView);

    setDefaultColorTable();

    setWindowTitle(tr("Grid Viewer"));
    resize(500, 400);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openAct_triggered()
{
    HorizonInputDialog* hdlg=new HorizonInputDialog;

    if( hdlg->exec()==QDialog::Accepted ){

        m_horizon=hdlg->horizon();

        gridView->setGrid( m_horizon );
        gridView->setColorMapping(std::make_pair(hdlg->minValue(), hdlg->maxValue()));
    }

    delete hdlg;

   // setDefaultColorTable();
}


void MainWindow::setDefaultColorTable(){

    GridView::ColorTable ct(256);

    for( int i=0; i<256; i++){
        ct[i]=QColor(i,0,255-i).rgb();
    }

    gridView->setColorTable( ct );
}

void MainWindow::on_zoomInAct_triggered()
{
    gridView->setScale( gridView->scale()*1.25);
}

void MainWindow::on_zoomOutAct_triggered()
{
    gridView->setScale( gridView->scale()*0.8);
}

void MainWindow::on_zoomNormalAct_triggered()
{
    gridView->setScale(1);
}

void MainWindow::on_displayRangeAct_triggered()
{

    std::pair<double, double> oldRange=gridView->colorMapping();

    DisplayRangeDialog* displayRangeDialog=new DisplayRangeDialog( oldRange);
    Q_ASSERT(displayRangeDialog);

    connect( displayRangeDialog, SIGNAL(displayRangeChanged(std::pair<double,double>)),
                 gridView, SLOT(setColorMapping(std::pair<double,double>)));

    if( displayRangeDialog->exec()==QDialog::Accepted ){
        gridView->setColorMapping( displayRangeDialog->range());
    }else{
        gridView->setColorMapping( oldRange );
    }

    delete displayRangeDialog;
}
