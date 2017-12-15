#ifndef SPECTRUMVIEW_H
#define SPECTRUMVIEW_H


#include<ruleraxisview.h>
#include<spectrum.h>


class SpectrumView : public RulerAxisView
{
Q_OBJECT

public:
    SpectrumView(QWidget* parent);

    std::shared_ptr<Spectrum> spectrum(){
        return m_spectrum;
    }

public slots:
    void setSpectrum( std::shared_ptr<Spectrum> );

protected:
    void refreshScene()override;

private:
    QPainterPath buildPath(const Spectrum&);

    std::shared_ptr<Spectrum> m_spectrum;
};

#endif // SPECTRUMVIEW_H
