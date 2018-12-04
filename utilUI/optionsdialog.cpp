#include "optionsdialog.h"

#include<QVBoxLayout>
#include<QDialogButtonBox>
#include<QLabel>
#include<QLineEdit>
#include<QIntValidator>
#include<QDoubleValidator>
#include<QSpinBox>
#include<QComboBox>
#include<QCheckBox>
#include<colorbutton.h>


OptionsDialog::OptionsDialog(QWidget* parent, Qt::WindowFlags flags):QDialog(parent,flags){

    m_controlLayout=new QGridLayout();

    auto bbox=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    connect(bbox,SIGNAL(accepted()),this,SLOT(accept()));
    connect(bbox,SIGNAL(rejected()),this,SLOT(reject()));

    auto lo=new QVBoxLayout();
    lo->addLayout(m_controlLayout);
    lo->addSpacing(20);
    lo->addStretch(1);
    lo->addWidget(bbox);

    setLayout(lo);
}

// returns -1 if not found
int OptionsDialog::indexOf(QString name){
    for( int i=0; i<m_controls.size(); i++){
        if(std::get<0>(m_controls[i])==name) return i;
    }
    return -1;
}

void OptionsDialog::clear(){
    m_controls.clear();
    for( auto w : layout()->children()){
        if( w ) delete w;
    }
};

void OptionsDialog::addItem(QString name, QVariant::Type itype, QVariant options, QString ltext){
    if(indexOf(name)>=0) return;
    QWidget* w=nullptr;
    switch(itype){
    case QVariant::String:
        w=createStringControl(options);
        break;
    case QVariant::Type::Int:
        w=createIntControl(options);
        break;
    case QVariant::Type::Double:
        w=createDoubleControl(options);
        break;
    case QVariant::Type::Bool:
        w=createBoolControl(options);
        break;
    case QVariant::Type::Color:
        w=createColorControl(options);
        break;
    default:
        break;
    }
    if(!w) return;

    //add new control to dialog/layout
    if(ltext.isEmpty()) ltext=name+":";
    auto l=new QLabel(ltext);
    l->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_controlLayout->addWidget(l,m_controls.size(),0);
    m_controlLayout->addWidget(w,m_controls.size(),1);
    m_controls.push_back(std::make_tuple(name,itype,w));
}

void OptionsDialog::removeItem(QString name){
    // find row
    int row=indexOf(name);
    if(row<0) return;
    auto l=m_controlLayout->itemAtPosition(row,0);
    m_controlLayout->removeItem(l);
    auto w=m_controlLayout->itemAtPosition(row,1);
    m_controlLayout->removeItem(w);
    m_controls.removeAt(row);
}

void OptionsDialog::setValue(QString name, QVariant value){
    auto row=indexOf(name);
    setValue(row, value);
}

void OptionsDialog::setValue(int row, QVariant value){
    if(row<0||row>=m_controls.size()) return;
    auto itype=std::get<1>(m_controls[row]);
    auto w=std::get<2>(m_controls[row]);

    auto le=dynamic_cast<QLineEdit*>(w);
    if(le){
        auto text=le->text();
        switch(itype){
        case QVariant::Type::String: le->setText(value.toString()); break;
        case QVariant::Type::Int: le->setText(QString::number(value.toInt())); break;
        case QVariant::Type::Double: le->setText(QString::number(value.toDouble())); break;
        default: break;
        }
    }

    auto cob=dynamic_cast<QComboBox*>(w);
    if(cob && itype==QVariant::Type::String){
        auto idx=cob->findText(value.toString());
        cob->setCurrentIndex(idx);
    }

    auto cb=dynamic_cast<QCheckBox*>(w);
    if(cb&&itype==QVariant::Type::Bool){
        cb->setChecked(value.toBool());
    }

    auto colb=dynamic_cast<ColorButton*>(w);
    if(colb&&itype==QVariant::Type::Color){
        colb->setColor(value.value<QColor>());
    }
}

void OptionsDialog::setValues(QList<QVariant> vals){
    if(vals.size()!=m_controls.size()) return;
    for(int i=0; i<m_controls.size(); i++){
        setValue(i,vals[i]);
    }
}

QVariant OptionsDialog::getValue(QString name){
    return getValue(indexOf(name));
}

QVariant OptionsDialog::getValue(int row){
    if(row<0||row>=m_controls.size()) return QVariant();
    auto itype=std::get<1>(m_controls[row]);
    auto w=std::get<2>(m_controls[row]);

    auto le=dynamic_cast<QLineEdit*>(w);
    if(le){
        auto text=le->text();
        switch(itype){
        case QVariant::Type::String: return QVariant(text); break;
        case QVariant::Type::Int: return QVariant(text.toInt()); break;
        case QVariant::Type::Double: return QVariant(text.toDouble()); break;
        default: return QVariant();break;
        }
    }

    auto cob=dynamic_cast<QComboBox*>(w);
    if(cob){
        switch(itype){
        case QVariant::Type::String: return QVariant(cob->currentText());break;
        default: return QVariant(); break;
        }
    }

    auto cb=dynamic_cast<QCheckBox*>(w);
    if(cb){
        switch(itype){
        case QVariant::Type::Bool: return QVariant(cb->isChecked());break;
        default: return QVariant();break;
        }
    }

    auto colb=dynamic_cast<ColorButton*>(w);
    if(colb){
        switch(itype){
        case QVariant::Type::Color: return QVariant(colb->color());break;
        default: return QVariant();break;
        }
    }

    return QVariant();
}

QList<QVariant> OptionsDialog::getValues(){
    QList<QVariant> vals;
    for(int i=0; i<m_controls.size(); i++){
        vals.push_back(getValue(i));
    }
    return vals;
}

QList<QVariant> OptionsDialog::getValues(QList<std::tuple<QString,QVariant,QVariant>> defs){
    OptionsDialog dlg;
    for( auto def : defs ){
        auto name=std::get<0>(def);
        auto value=std::get<1>(def);
        auto options=std::get<2>(def);
        dlg.addItem(name,value.type(),options);
        dlg.setValue(name,value);
    }
    if( dlg.exec()!=QDialog::Accepted) return QList<QVariant>();
    return dlg.getValues();
}


QWidget* OptionsDialog::createStringControl(QVariant options){
    if(options.type()==QVariant::Type::StringList){
        auto cb=new QComboBox();
        cb->setEditable(false);
        cb->insertItems(0,options.toStringList());
        return cb;
    }
    else{
        auto le=new QLineEdit();
        return le;
    }
}

QWidget* OptionsDialog::createIntControl(QVariant){
    auto le=new QLineEdit();
    auto validator=new QIntValidator();
    le->setValidator(validator);
    return le;
}

QWidget* OptionsDialog::createDoubleControl(QVariant){
    auto le=new QLineEdit();
    auto* validator=new QDoubleValidator();
    le->setValidator(validator);
    return le;
}

QWidget* OptionsDialog::createBoolControl(QVariant){
    auto cb=new QCheckBox("");
    return cb;
}

QWidget* OptionsDialog::createColorControl(QVariant){
    auto cb=new ColorButton();
    connect(cb,SIGNAL(clicked()),cb,SLOT(chooseColor()));
    return cb;
}
