#include "selectheaderwordsdialog.h"

#include<QGridLayout>
#include<QBoxLayout>
#include<QLabel>

SelectHeaderWordsDialog::SelectHeaderWordsDialog(int n, QWidget *parent) :
    QDialog(parent), comboBoxes(n)
{


    QVBoxLayout* mainLayout=new QVBoxLayout;

    QLayout* comboBoxLayout=     createAndLayoutComboBoxes();
    QLayout* buttonLayout=          createAndLayoutButtons();

    mainLayout->addItem(comboBoxLayout);
    mainLayout->addItem(buttonLayout);

    setLayout(mainLayout);

    createConnections();
}

SelectHeaderWordsDialog::~SelectHeaderWordsDialog()
{

}


void SelectHeaderWordsDialog::setHeaderWords( const QStringList& list){

    for( int i=0; i<comboBoxes.size(); i++){
        comboBoxes[i]->clear();
        comboBoxes[i]->addItems(list);
    }

}


QString SelectHeaderWordsDialog::selectedHeaderWord(int i)const{

    if( i<0 || i>=comboBoxes.size()){
        qFatal( "Headerword index out of range!!!");
    }

    return comboBoxes[i]->currentText();
}

QLayout* SelectHeaderWordsDialog::createAndLayoutComboBoxes(){

    QGridLayout* lo=new QGridLayout;

    for( int i=0; i<comboBoxes.size(); i++){

        QLabel* lbl=new QLabel( QString::asprintf("Header Word #%d:", i+1), this);
        comboBoxes[i]=new QComboBox(this);

        lo->addWidget(lbl, i, 0);
        lo->addWidget( comboBoxes[i], i, 1);
    }

    return lo;
}

QLayout* SelectHeaderWordsDialog::createAndLayoutButtons(){

    btOk=new QPushButton("Ok", this);
    btCancel=new QPushButton("Cancel", this);

    QHBoxLayout* buttonLayout=new QHBoxLayout;

    buttonLayout->addWidget(btCancel);
    buttonLayout->addWidget(btOk);

    return buttonLayout;
}


void SelectHeaderWordsDialog::createConnections(){

    connect( btOk, SIGNAL(clicked()), this, SLOT(accept()) );
    connect( btCancel, SIGNAL( clicked()), this, SLOT(reject()));
}


