#include "headerdialog.h"
#include "ui_headerdialog.h"
#include "navigationwidget.h"
#include "headerui.h"
#include<algorithm>
#include<iostream>

#include<QStandardItemModel>
#include<QStandardItem>
#include<QTableWidgetItem>



HeaderDialog::HeaderDialog(QWidget *parent, bool navigate) :
    QDialog(parent),
    ui(new Ui::HeaderDialog)
{
    ui->setupUi(this);

    if(!navigate) ui->navigationWidget->hide();
}

HeaderDialog::~HeaderDialog()
{
    delete ui;
}


// this cannot be in header file because of reference to ui
NavigationWidget* HeaderDialog::navigationWidget(){
    return ui->navigationWidget;
}


void HeaderDialog::setData( const seismic::Header& hdr, const std::vector<seismic::SEGYHeaderWordDef>& hdef){

    m_hdr=hdr;
    m_hdef=hdef;

    if( !model ){  // create model from header

        int nrows=hdr.size();
        model = new QStandardItemModel(nrows, 5, this);

        model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Position"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("Size"));
        model->setHeaderData(3, Qt::Horizontal, QObject::tr("Type"));
        model->setHeaderData(4, Qt::Horizontal, QObject::tr("Value"));

        int row=0;
        for( auto v : hdr ){

            model->setItem(row, 0, new QStandardItem(QString( v.first.c_str())));
            model->setItem(row, 4, new QStandardItem(toQString(v.second)));

            std::vector<seismic::SEGYHeaderWordDef>::iterator it_def=std::find_if(m_hdef.begin(), m_hdef.end(), [&](const seismic::SEGYHeaderWordDef d) {
                return d.name == v.first;
            });
            if( it_def==m_hdef.end()){
                qFatal("Header name not found in header definition!!!");
            }
            seismic::SEGYHeaderWordDef def=*it_def;

            QStandardItem* item1 = new QStandardItem;
            // put a QVariant(int) into the DisplayRole rather than a string
            item1->setData((int)def.pos, Qt::DisplayRole);
            model->setItem(row,1,item1);


            model->setItem(row, 2, new QStandardItem(toQString(def.dtype)));
            model->setItem(row, 3, new QStandardItem(toQString(def.ctype)));

             row++;
        }

        ui->tableView->setModel(model);

        // sort by position
        ui->tableView->setSortingEnabled(true);
        ui->tableView->sortByColumn(1,Qt::AscendingOrder);

        connect(
          ui->tableView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this,
          SLOT(tableSelectionChanged(QItemSelection,QItemSelection))
         );

        ui->tableView->setCurrentIndex(model->index(0,0));

    }
    else{  // update model from header

        for( int row=0; row<model->rowCount(); row++ ){

            QStandardItem* nameItem=model->item( row, 0);
            QString keyName=nameItem->text();//data().toString();
            std::string key=keyName.toStdString();
            if( hdr.find(key)==hdr.end()) continue;         // ignore missing keys
            seismic::HeaderValue value=hdr.at( key);
            QStandardItem* valueItem=model->item( row, 4);
            valueItem->setText(toQString(value));
        }

    }
}

void HeaderDialog::tableSelectionChanged( const QItemSelection& new_item, const QItemSelection&){
    int irow=new_item.indexes().first().row();

    QString name=ui->tableView->model()->index(irow,0).data().toString();
    std::string sname=name.toStdString();
    std::vector<seismic::SEGYHeaderWordDef>::iterator it_def=std::find_if(m_hdef.begin(), m_hdef.end(), [&](const seismic::SEGYHeaderWordDef d) {
        return d.name == sname;
    });
    if( it_def==m_hdef.end()){
        qFatal("Header name not found in header definition!!!");
    }
    ui->textEdit->setText(it_def->descr.c_str());
}



void HeaderDialog::on_pbClose_clicked()
{
    emit finished();
    hide();
}
