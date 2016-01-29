#include "navigationwidget.h"
#include "ui_navigationwidget.h"

#include<QKeyEvent>
//#include<iostream>

NavigationWidget::NavigationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavigationWidget)
{
    ui->setupUi(this);

    m_validator=new QIntValidator( m_min, m_max, this );
    ui->leCurrent->setValidator(m_validator);
}

NavigationWidget::~NavigationWidget()
{
    delete ui;
}

void NavigationWidget::setRange( size_t rmin, size_t rmax){

    if( rmin==m_min && rmax==m_max ) return;
    m_min=rmin;
    m_max=rmax;

    //update validator
    m_validator->setRange(m_min, m_max);

    //update controls
    ui->leMaximum->setText(QString::number(m_max));
    emit(rangeChanged(rmin, rmax ) );

    // adjust current to range
    if( m_current<m_min ) setCurrent(m_min);
    if( m_current>m_max ) setCurrent(m_max);

}

void NavigationWidget::setCurrent( size_t cur ){

    if( cur < m_min ) cur=m_min;
    if( cur > m_max ) cur=m_max;

    if( cur==m_current ) return;

    m_current=cur;

    emit(currentChanged(cur) );

    ui->leCurrent->setText(QString::number(cur));

}


void NavigationWidget::keyPressEvent(QKeyEvent *ev){
    if( ev->key() == Qt::Key_Return){
        ev->accept();
        //std::cout<<"ATE IT"<<std::endl;
    }
    else{
        QWidget::keyPressEvent(ev);
    }

}

void NavigationWidget::on_pbFirst_clicked()
{
    setCurrent(m_min);
}

void NavigationWidget::on_pbLast_clicked()
{
    setCurrent(m_max);
}

void NavigationWidget::on_pbPrevious_clicked()
{
    if(m_current>0){
        setCurrent(m_current-1);
    }
}

void NavigationWidget::on_pbNext_clicked()
{
    setCurrent(m_current+1);
}

void NavigationWidget::on_leCurrent_returnPressed()
{
    setCurrent( ui->leCurrent->text().toUInt());
}
