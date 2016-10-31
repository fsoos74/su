#ifndef SECTIONSCALEDIALOG_H
#define SECTIONSCALEDIALOG_H

#include <QDialog>

namespace Ui {
class SectionScaleDialog;
}


/*
 * Use pixel per trace/second for signal/slots but display mm per trace/second
*/

class SectionScaleDialog : public QDialog
{
    Q_OBJECT

public:

    enum Unit{ CENTIMETER=0, INCH};

    explicit SectionScaleDialog(QWidget *parent = 0);
    ~SectionScaleDialog();

    qreal dpiX()const{
        return m_dpiX;
    }

    qreal dpiY()const{
        return m_dpiY;
    }

    qreal pixelPerTrace()const{
        return m_pixelPerTrace;
    }

    qreal pixelPerSecond()const{
        return m_pixelPerSecond;
    }

    bool isLocked();

    Unit unit()const{
        return m_unit;
    }

public slots:
    void setDPIX(qreal);
    void setDPIY(qreal);
    void setPixelPerTrace(qreal);
    void setPixelPerSecond(qreal);
    void setLocked(bool);
    void setUnit( Unit );

signals:
    void dpiXChanged(qreal);
    void dpiYChanged(qreal);
    void pixelPerTraceChanged(qreal);
    void pixelPerSecondChanged(qreal);
    void lockedChanged(bool);
    void unitChanged( Unit );

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);

private slots:

    void pixelPerTraceFromControl();
    void pixelPerTraceToControl();
    void pixelPerSecondFromControl();
    void pixelPerSecondToControl();
    void unitToControl();
    void unitFromControl();

    void apply();

    void on_cbUnit_currentIndexChanged(int index);

private:
    Ui::SectionScaleDialog *ui;

    qreal m_dpiX=1;
    qreal m_dpiY=1;
    qreal m_pixelPerTrace=1;
    qreal m_pixelPerSecond=1;
    Unit m_unit=Unit::INCH;
};

#endif // SECTIONSCALEDIALOG_H
