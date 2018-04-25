#include "logeditdialog.h"
#include "ui_logeditdialog.h"

#include <qstandarditemmodel.h>

LogEditDialog::LogEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogEditDialog)
{
    ui->setupUi(this);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

LogEditDialog::~LogEditDialog()
{
    delete ui;
}

const Log& LogEditDialog::log(){
   model2log();
   return m_log;
}

void LogEditDialog::setLog(const Log &log){
    m_log=log;
    log2model();
}


// this allows numeric columns to be sorted correctly
template<typename T>
QStandardItem* newAlignedItem( const T& t, Qt::Alignment align=Qt::AlignCenter ){
    QStandardItem* item=new QStandardItem;
    if( !item ) return item;
    item->setData( QVariant(t), Qt::DisplayRole);
    item->setTextAlignment(align);
    return item;
}

void LogEditDialog::log2model(){

    QStringList labels;
    labels<<"MD"<<"Value";
    QStandardItemModel* model=new QStandardItemModel(m_log.nz(), labels.size(), this);
    model->setHorizontalHeaderLabels(labels);

    for( auto iz = 0; iz<m_log.nz(); iz++){

        auto md=m_log.index2z(iz);
        auto value=m_log[iz];
        auto mditem=newAlignedItem(md, Qt::AlignRight);
        mditem->setFlags(mditem->flags() ^ Qt::ItemIsEditable); // read only
        model->setItem(iz, 0, mditem );
        if( value!=m_log.NULL_VALUE){
            auto item=newAlignedItem(QString::number(value), Qt::AlignRight);       // qstring allows empty string for null values
            model->setItem(iz, 1, item );
        }
    }



    ui->tableView->setModel(model);
}


void LogEditDialog::model2log(){

    auto model = ui->tableView->model();

    for( auto iz = 0; iz<model->rowCount(); iz++){

        auto value=m_log.NULL_VALUE;

        auto idx=model->index(iz,1);
        if( ! model->data(idx).toString().isEmpty()){ // NO NULL VALUE
            bool ok=false;
            auto tmp=model->data(idx).toDouble(&ok);    // invalid number will result in NULL Value
            if( ok ) value=tmp;
        }

        m_log[iz]=value;
    }
}
