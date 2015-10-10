#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QKeyEvent>

#include<memory>
#include<utility>

#include "segyreader.h"

/*
namespace seismic{
    class SEGYReader;
    class SEGYHeaderWordDef;
}
*/

class EBCDICDialog;
class HeaderDialog;


namespace Ui {
class SegyInputDialog;
}

class SegyInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SegyInputDialog(QWidget *parent = 0 );
    ~SegyInputDialog();

    seismic::SEGYReader* reader();

protected:

    virtual void keyPressEvent(QKeyEvent*);

private slots:

    void on_btScan_clicked();

    void on_pbBrowse_clicked();

    void on_pbEBCDIC_clicked();

    void on_pbBinary_clicked();

    void on_pbTraceHeader_clicked();

    void on_leFilename_returnPressed();

    void on_pbOK_clicked();

    void readerParamsChanged();

    void onTraceHeaderDialogFinished();

    void navigateToTraceHeader( int );

private:

    std::vector<seismic::SEGYHeaderWordDef> getTraceHeaderDefinition();
    void openReader();
    void destroyReader();
    void updateButtons();
    void updateEBCDICDialog();
    void updateBinaryDialog();
    void updateTraceHeaderDialog();

    void apply();
    void showTraceHeader(ssize_t n);

    Ui::SegyInputDialog *ui;

    EBCDICDialog* m_EBCDICDialog=nullptr;
    HeaderDialog* m_BinaryDialog=nullptr;
    HeaderDialog* m_TraceHeaderDialog=nullptr;

    seismic::SEGYReader* m_reader=nullptr;
};

#endif // DIALOG_H
