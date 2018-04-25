#include "tabledialog.h"
#include "ui_tabledialog.h"

#include<QStandardItemModel>
#include<QStandardItem>
#include<QMessageBox>


TableDialog::TableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableDialog)
{
    ui->setupUi(this);

    connect(ui->pbSave, SIGNAL(clicked(bool)), this, SLOT(saveTable()));
}

TableDialog::~TableDialog()
{
    delete ui;
}

void TableDialog::setProject(AVOProject *project){
    if(project==m_project) return;
    m_project=project;
    ui->tableWidget->clear();
}

void TableDialog::loadTable(QString name){

    if( !m_project ) return;

    if( name==m_name) return;

    auto table=m_project->loadTable(name);
    if( !table ){
        QMessageBox::critical(this, "Load Table", tr("Loading table \"%1\" failed!").arg(name));
        return;
    }

    tableToView(*table);

    m_name=name;

    setWindowTitle(tr("Table - %1").arg(name));
}

void TableDialog::saveTable(){
    if(!m_project) return;
    if(m_name.isEmpty()) return;
    if(ui->tableWidget->rowCount()<1) return;
    auto table=tableFromView();
    if(!m_project->saveTable(m_name, table)){
        QMessageBox::critical(this, "Save Table", tr("Saving table \"%1\" failed!").arg(m_name));
        return;
    }
}

void TableDialog::tableToView(const Table &table){

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(table.size());
    ui->tableWidget->setColumnCount(3);
    auto model=ui->tableWidget->model();

    QStringList labels;
    labels<<table.key1()<<table.key2()<<"value";
    ui->tableWidget->setHorizontalHeaderLabels(labels);

    int row=0;
    for( auto it=table.cbegin(); it!=table.cend(); it++){

        auto key=table.split_key(it.key());
        auto k1=key.first;
        auto k2=key.second;
        auto value=it.value();
        model->setData(model->index(row,0), k1);
        model->setData(model->index(row,1), k2);
        model->setData(model->index(row,2), value);
        row++;
    }

    m_multi=table.isMulti();        // MUST STORE THIS!!!!

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}


Table TableDialog::tableFromView(){

    auto model=ui->tableWidget->model();
    auto key1=model->headerData(0, Qt::Horizontal).toString();
    auto key2=model->headerData(1, Qt::Horizontal).toString();
    Table  tmp(key1, key2, m_multi);


    for( int i=0; i<model->rowCount(); i++){
        auto key1 = model->data(model->index(i,0)).toInt();
        auto key2 = model->data(model->index(i,1)).toInt();
        auto value = model->data(model->index(i,2)).toDouble();
        tmp.insert(key1, key2, value);
    }

    return tmp;
}


void TableDialog::on_pbAdd_clicked()
{
    auto row=ui->tableWidget->currentRow();
    ui->tableWidget->insertRow(row);
    ui->tableWidget->scrollTo(ui->tableWidget->model()->index(row, 0));
}

void TableDialog::on_pbRemove_clicked()
{
    auto selection=ui->tableWidget->selectionModel()->selectedRows(0);
    std::vector<int> rows;
    for( auto idx: selection ){
        auto row=idx.row();
        rows.push_back(row);
    }

    std::sort( rows.begin(), rows.end() );

    for( auto it = rows.rbegin(); it!=rows.rend(); ++it ){
        ui->tableWidget->removeRow(*it);
    }
}
