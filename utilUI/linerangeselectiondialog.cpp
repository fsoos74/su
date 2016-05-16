#include "linerangeselectiondialog.h"
#include "ui_linerangeselectiondialog.h"


#include<QIntValidator>
#include<QPushButton>


LineRangeSelectionDialog::LineRangeSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LineRangeSelectionDialog)
{
    ui->setupUi(this);

    QIntValidator* validator=new QIntValidator(this);
    ui->leMinInline->setValidator(validator);
    ui->leMaxInline->setValidator(validator);
    ui->leMinCrossline->setValidator(validator);
    ui->leMaxCrossline->setValidator(validator);

    connect( ui->leMinInline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMaxInline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMinCrossline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
    connect( ui->leMaxCrossline, SIGNAL(textChanged(QString)), this, SLOT(checkOk()) );
}

LineRangeSelectionDialog::~LineRangeSelectionDialog()
{
    delete ui;
}



int LineRangeSelectionDialog::minInline()const{

    return ui->leMinInline->text().toInt();
}

int LineRangeSelectionDialog::maxInline()const{

    return ui->leMaxInline->text().toInt();
}

int LineRangeSelectionDialog::minCrossline()const{

    return ui->leMinCrossline->text().toInt();
}

int LineRangeSelectionDialog::maxCrossline()const{

    return ui->leMaxCrossline->text().toInt();
}


void LineRangeSelectionDialog::setMinInline(int line){

    ui->leMinInline->setText(QString::number(line));
}

void LineRangeSelectionDialog::setMaxInline(int line){

    ui->leMaxInline->setText(QString::number(line));
}

void LineRangeSelectionDialog::setMinCrossline(int line){

    ui->leMinCrossline->setText(QString::number(line));
}

void LineRangeSelectionDialog::setMaxCrossline(int line){

    ui->leMaxCrossline->setText(QString::number(line));
}

void LineRangeSelectionDialog::checkOk(){

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
                minInline()<=maxInline() && minCrossline()<=maxCrossline());
}
