#ifndef GATHERSCALER_H
#define GATHERSCALER_H

#include<QObject>
#include<gather.h>
#include<memory>
#include<QVector>

class GatherScaler : public QObject
{
    Q_OBJECT


public:
    enum class Mode{ NoScaling, Individual, Entire, Fixed };

    GatherScaler(QObject* parent=nullptr);

    Mode mode()const{
        return m_mode;
    }

    qreal fixedScaleFactor()const{
        return m_fixedScaleFactor;
    }

    QVector<qreal> computeScalingFactors( std::shared_ptr<seismic::Gather>);

public slots:

    void setMode(GatherScaler::Mode);
    void setFixedScaleFactor(qreal);

signals:
    void changed();
    void fixedScaleFactorChanged(qreal);

private:


    Mode m_mode=Mode::Entire; // Mode::Individual;

    qreal m_fixedScaleFactor=2.5;

};

#endif // GATHERSCALER_H
