#include "headerdialog.h"
#include "ui_headerdialog.h"
#include "navigationwidget.h"

#include<algorithm>

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


QString toQString( const seismic::HeaderValue& v){
    switch( v.type()){
    case seismic::HeaderValue::Type::INT_VALUE:
        return QString::number(v.intValue());
        break;
    case seismic::HeaderValue::Type::UINT_VALUE:
        return QString::number(v.uintValue());
        break;
    case seismic::HeaderValue::Type::FLOAT_VALUE:
        return QString::number(v.floatValue());
        break;
    default:
        qFatal("Invalid header value format!!!");
        break;
    }
}

QString toQString( const seismic::SEGYHeaderWordDataType& t){
    switch( t ){
    case  seismic::SEGYHeaderWordDataType::SEGY_INT16:
        return QString("INT16");
        break;
    case  seismic::SEGYHeaderWordDataType::SEGY_INT32:
        return QString("INT32");
        break;
    case  seismic::SEGYHeaderWordDataType::SEGY_UINT16:
        return QString("UINT16");
        break;
    case  seismic::SEGYHeaderWordDataType::SEGY_BUF6:
        return QString("6 bytes");
        break;
    default:
        qFatal("Invalid header value type!!!");
        break;
    }
}

QString toQString( const seismic::SEGYHeaderWordConvType& t){
    switch( t ){
    case  seismic::SEGYHeaderWordConvType::SEGY_PLAIN:
        return QString("plain value");
        break;
    case   seismic::SEGYHeaderWordConvType::SEGY_ELEV:
        return QString("elevation");
        break;
    case   seismic::SEGYHeaderWordConvType::SEGY_COORD:
        return QString("coordinate");
        break;
    default:
        qFatal("Invalid header value conversion type!!!");
        break;
    }
}

void HeaderDialog::setData( const seismic::Header& hdr, const std::vector<seismic::SEGYHeaderWordDef>& hdef){

    m_hdr=hdr;
    m_hdef=hdef;

    int nrows=hdr.size();
    QStandardItemModel *model = new QStandardItemModel(nrows, 5, this);

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

void HeaderDialog::tableSelectionChanged( const QItemSelection& new_item, const QItemSelection& old_item){
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
