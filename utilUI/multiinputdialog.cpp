#include "multiinputdialog.h"

#include<QGridLayout>
#include <QLabel>
#include<QDialogButtonBox>
#include<QIntValidator>
#include<QDoubleValidator>


MultiInputDialog::MultiInputDialog( int count, QStringList labels, QStringList inputs, InputStyle istyle,  QWidget* parent)
    : QDialog(parent), m_istyle(istyle)
{

    QGridLayout* lo=new QGridLayout;

    for( int i=0; i<count; i++){

        auto label = new QLabel(this);
        if( labels.size()>i) label->setText(labels[i]);
        lo->addWidget( label, i, 0);

        if( istyle==InputStyle::LineEdit){
            auto lineEdit = new QLineEdit(this);
            if( inputs.size()>i) lineEdit->setText(inputs[i]);
            m_lineEdits.push_back(lineEdit);
            lo->addWidget( lineEdit, i, 1);
        }
        else{
            auto cb = new QComboBox(this);
            for( auto s : inputs ){
                cb->addItem(s);
            }
            cb->setEditable(false);
            m_combos.push_back(cb);
            lo->addWidget( cb, i, 1);
        }
    }

    auto bb = new QDialogButtonBox(this);
    bb->addButton(QDialogButtonBox::Ok);
    bb->addButton(QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));
    lo->addWidget(bb, count, 0, 1, 2, Qt::AlignRight);

    setLayout(lo);
}

QStringList MultiInputDialog::getInputs(){

    QStringList res;

    if( m_istyle==InputStyle::LineEdit){
        for( auto le : m_lineEdits){
            res.append(le->text());
        }
    }
    else{
        for( auto cb : m_combos){
            res.append( cb->currentText());
        }
    }

    return res;
}


QStringList MultiInputDialog::getTexts( QWidget* parent, QString title, QStringList labels, QStringList texts, bool* ok){

    QStringList res;
    bool ac=false;

    MultiInputDialog* dlg=new MultiInputDialog( labels.size(), labels, texts, InputStyle::LineEdit, parent );
    dlg->setWindowTitle(title);
    if( dlg->exec()==QDialog::Accepted){
        ac=true;
        res=dlg->getInputs();
    }

    if( ok ){
        *ok=ac;
    }

    delete dlg;

    return res;
}

QVector<int> MultiInputDialog::getInts( QWidget* parent, QString title, QStringList labels, QVector<int> vals, bool* ok){

    QVector<int> res;
    bool ac=false;

    QStringList texts;
    for( auto v: vals ){
        texts.append(QString::number(v));
    }

    MultiInputDialog* dlg=new MultiInputDialog( labels.size(), labels, texts, InputStyle::LineEdit, parent );
    dlg->setWindowTitle(title);
    QIntValidator* validator=new QIntValidator(dlg);
    for( auto le : dlg->m_lineEdits){
        le->setValidator(validator);
    }

    if( dlg->exec()==QDialog::Accepted){
        ac=true;
        auto texts=dlg->getInputs();
        for( auto s : texts){
            res.push_back(s.toInt());
        }
    }

    delete dlg;

    if( ok ){
        *ok=ac;
    }

    return res;
}


QVector<double> MultiInputDialog::getDoubles( QWidget* parent, QString title, QStringList labels, QVector<double> vals, bool* ok){

    QVector<double> res;
    bool ac=false;

    QStringList texts;
    for( auto v: vals ){
        texts.append(QString::number(v));
    }

    MultiInputDialog* dlg=new MultiInputDialog( labels.size(), labels, texts, InputStyle::LineEdit, parent );
    dlg->setWindowTitle(title);
    QDoubleValidator* validator=new QDoubleValidator(dlg);
    for( auto le : dlg->m_lineEdits){
        le->setValidator(validator);
    }

    if( dlg->exec()==QDialog::Accepted){
        ac=true;
        auto texts=dlg->getInputs();
        for( auto s : texts){
            res.push_back(s.toDouble());
        }
    }

    delete dlg;

    if( ok ){
        *ok=ac;
    }

    return res;
}


QStringList MultiInputDialog::getItems( QWidget* parent, QString title, QStringList labels, QStringList items, bool* ok){

    QStringList res;
    bool ac=false;

    MultiInputDialog* dlg=new MultiInputDialog( labels.size(), labels, items, InputStyle::ComboBox, parent );
    dlg->setWindowTitle(title);

    if( dlg->exec()==QDialog::Accepted){
        ac=true;
        res=dlg->getInputs();
    }

    delete dlg;

    if( ok ){
        *ok=ac;
    }

    return res;
}
