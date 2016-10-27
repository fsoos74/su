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

public slots:
    void setDPIX(qreal);
    void setDPIY(qreal);
    void setPixelPerTrace(qreal);
    void setPixelPerSecond(qreal);

signals:
    void dpiXChanged(qreal);
    void dpiYChanged(qreal);
    void pixelPerTraceChanged(qreal);
    void pixelPerSecondChanged(qreal);

protected:
    // override this to keep return pressed in line edit move up to parent
    void keyPressEvent(QKeyEvent* event);

private slots:

    void pixelPerTraceFromControl();
    void pixelPerTraceToControl();
    void pixelPerSecondFromControl();
    void pixelPerSecondToControl();

    void apply();

private:
    Ui::SectionScaleDialog *ui;

    qreal m_dpiX=1;
    qreal m_dpiY=1;
    qreal m_pixelPerTrace=1;
    qreal m_pixelPerSecond=1;
};

#endif // SECTIONSCALEDIALOG_H
