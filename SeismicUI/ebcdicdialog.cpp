#include "ebcdicdialog.h"
#include "ui_ebcdicdialog.h"

#include "segy_text_header.h"

EBCDICDialog::EBCDICDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EBCDICDialog)
{
    ui->setupUi(this);
    QFontMetrics fm(ui->teHeader->font());
    int w=fm.width(QString("12345678901234567890123456789012345678901234567890123456789012345678901234567890")); // 80 chars per line
    ui->teHeader->setMinimumWidth(w);
}

EBCDICDialog::~EBCDICDialog()
{
    delete ui;
}

void EBCDICDialog::setHeader( const seismic::SEGYTextHeader& hdr){
    m_hdr=hdr;
    updateText();
}

void EBCDICDialog::updateText(){

    QString txt;
    seismic::SEGYTextHeaderStr ths=seismic::convertToString(m_hdr, ui->rbEBCDIC->isChecked());

    for( auto line : ths ){
        txt.append(QString(line.c_str()));
        txt.append("\n");
    }
    ui->teHeader->setLineWidth(80);
    ui->teHeader->clear();
    ui->teHeader->append(txt);

    ui->teHeader -> moveCursor (QTextCursor::Start) ;
    ui->teHeader -> ensureCursorVisible() ;


}

void EBCDICDialog::on_rbEBCDIC_toggled(bool)
{
    updateText();
}
