#ifndef SPECTRUMVIEWER_H
#define SPECTRUMVIEWER_H

#include <baseviewer.h>

#include<avoproject.h>
#include <spectrum.h>
#include "spectrumdefinition.h"
#include "spectrumdataselectiondialog.h"

#include<QMap>
#include<QColor>

namespace Ui {
class SpectrumViewer;
}

class SpectrumViewer : public BaseViewer
{
    Q_OBJECT

public:
    explicit SpectrumViewer(QWidget *parent = 0);
    ~SpectrumViewer();

public slots:
    void setProject( AVOProject*);
    void addSpectrum( SpectrumDefinition );
    void removeSpectrum( int index );
    void changeColor( int index );

protected:
    void receivePoint( SelectionPoint, int code );
    void receivePoints( QVector<SelectionPoint>, int code);

private slots:
    void on_action_Add_Spectrum_triggered();
    void runTableViewContextMenu(const QPoint&);

private:

    struct Item{
        SpectrumDefinition info;
        Spectrum spectrum;
        QColor color;
    };

    void updateScene();
    void updateTable();
    void showSelector();  // also creates it if necessary
    Spectrum generateSpectrum( SpectrumDefinition );

    Ui::SpectrumViewer *ui;
    SpectrumDataSelectionDialog* selector=nullptr;
    AVOProject* m_project=nullptr;

    QMap< int, Item > m_spectra;
};

#endif // SPECTRUMVIEWER_H
