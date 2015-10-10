#ifndef EBCDICDIALOG_H
#define EBCDICDIALOG_H

#include <QDialog>

/*
namespace seismic{
    class SEGYTextHeader;
}
*/

#include<segy_text_header.h>

namespace Ui {
class EBCDICDialog;
}

class EBCDICDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EBCDICDialog(QWidget *parent = 0);
    ~EBCDICDialog();

public slots:
    void setHeader( const seismic::SEGYTextHeader& hdr);

private slots:
    void on_rbEBCDIC_toggled(bool checked);

private:
    void updateText();

    Ui::EBCDICDialog *ui;
    seismic::SEGYTextHeader m_hdr;
};

#endif // EBCDICDIALOG_H
