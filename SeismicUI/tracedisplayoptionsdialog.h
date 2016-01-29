#ifndef TRACEDISPLAYOPTIONSDIALOG_H
#define TRACEDISPLAYOPTIONSDIALOG_H

#include <QDialog>

#include "gatherlabel.h"  // display mode


#include <colortabledialog.h>


namespace Ui {
class TraceDisplayOptionsDialog;
}

class TraceDisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TraceDisplayOptionsDialog(QWidget *parent = 0);
    ~TraceDisplayOptionsDialog();

    bool isDisplayWiggles();
    bool isDisplayVariableArea();
    bool isDisplayDensity();
    QColor traceColor();
    int traceOpacity();
    int densityOpacity();

public slots:
    void setDisplayWiggles( bool );
    void setDisplayVariableArea( bool );
    void setDisplayDensity( bool );
    void setColors( QVector<QRgb>);
    void setTraceColor(QColor);
    void setTraceOpacity(int);
    void setDensityOpacity(int);

signals:
    void displayWigglesChanged( bool );
    void displayVariableAreaChanged( bool );
    void displayDensityChanged( bool );
    void colorsChanged(QVector<QRgb>);
    void traceColorChanged(QColor);
    void traceOpacityChanged(int);
    void densityOpacityChanged(int);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);


private slots:
    void on_pbColorTable_clicked();

    void on_cbWiggles_toggled(bool checked);

    void on_cbVariableArea_toggled(bool checked);

    void on_cbDensity_toggled(bool checked);

    void on_cbTraceColor_clicked();

    void on_sbTraceOpacity_valueChanged(int arg1);

private:
    Ui::TraceDisplayOptionsDialog *ui;
    QVector<QRgb> m_colors;
};

#endif // TRACEDISPLAYOPTIONSDIALOG_H
