#ifndef VOLUMEDIPPROCESS_H
#define VOLUMEDIPPROCESS_H

#include "projectprocess.h"
#include <QObject>
#include<grid3d.h>
#include<memory>
#include<functional>

class VolumeDipProcess : public ProjectProcess
{

    Q_OBJECT

public:

    enum class Method{ DERIVATION, DERIVATION_5P, DERIVATION_7P, CORRELATION, MAXIMUM_SEMBLANCE};

    VolumeDipProcess( AVOProject* project, QObject* parent=nullptr);

    ResultCode init(const QMap<QString, QString>& parameters);
    ResultCode run();

    static QString toQString(Method);
    static Method toMethod(QString);
    static QStringList methodList();

private:

    std::pair<float, float> dips_derivative( int i, int j, int k);
    std::pair<float, float> dips_derivative_5p( int i, int j, int k);
    std::pair<float, float> dips_derivative_7p( int i, int j, int k);
    std::pair<float, float> dips_correlation( int i, int j, int k);
    std::pair<float, float> dips_maximum_semblance( int i, int j, int k);

    QString m_ilineDipName;
    QString m_xlineDipName;
    std::shared_ptr<Volume > m_inputVolume;
    std::shared_ptr<Volume > m_ilineDipVolume;
    std::shared_ptr<Volume > m_xlineDipVolume;

    std::function< std::pair<float, float>( int, int, int ) > m_func;
    int m_margin=0;
    int m_window_samples;
};

#endif // VOLUMEDIPPROCESS_H
