#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QKeyEvent>

#include<memory>
#include<utility>

#include "segyreader.h"
#include "segyinfo.h"

/*
namespace seismic{
    class SEGYReader;
    class SEGYHeaderWordDef;
}
*/

class EBCDICDialog;
class HeaderDialog;
class HeaderScanDialog;

namespace Ui {
class SegyInputDialog;
}

class SegyInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SegyInputDialog(QWidget *parent = 0 );
    ~SegyInputDialog();


    std::shared_ptr<seismic::SEGYReader> reader()const{
        return m_reader;
    }

    const seismic::SEGYInfo& info()const{
        return m_info;
    }

    QString path()const;

public slots:

    void setInfo(const seismic::SEGYInfo& info);

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

    void on_pbScanTraceHeaders_clicked();

private:

    void updateControlsFromInfo();
    void updateInfoFromControls();
    void readInfoFile();
    void writeInfoFile();

    std::vector<seismic::SEGYHeaderWordDef> getTraceHeaderDefinition();
    void openReader();
    void destroyReader();
    void updateButtons();
    void updateControlsFromReader();
    void updateEBCDICDialog();
    void updateBinaryDialog();
    void updateTraceHeaderDialog();

    void apply();
    void showTraceHeader(ssize_t n);

    QString infoFileName();

    Ui::SegyInputDialog *ui;

    EBCDICDialog* m_EBCDICDialog=nullptr;
    HeaderDialog* m_BinaryDialog=nullptr;
    HeaderDialog* m_TraceHeaderDialog=nullptr;
    HeaderScanDialog* m_HeaderScanDialog=nullptr;

    seismic::SEGYInfo m_info;
    std::shared_ptr<seismic::SEGYReader> m_reader;

    bool m_updateReaderParamsEnabled=true;
    bool m_infoDirty=false;

};

#endif // DIALOG_H
