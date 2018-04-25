#ifndef SMOOTHPROCESSOR_H
#define SMOOTHPROCESSOR_H

#include <vector>
#include <functional>
#include <QString>


class SmoothProcessor
{
public:

    enum class OP{ MEAN, MEAN80, MEAN_DISTANCE_WEIGHTED, MEDIAN, MINIMUM, MAXIMUM };

    static const double NULL_VALUE;

    SmoothProcessor();

    OP op()const{
        return m_op;
    }

    double inputNullValue(){
        return m_inputNullValue;
    }

    void setOP(OP);
    void setInputNullValue(const double&);

    void clearInput();
    void addInput(const double& value, const double& dist=0);

    double compute();

    static QStringList opList();
    static QString toQString(OP);
    static OP toOP(QString);

private:

    double mean();
    double mean_80();
    double mean_distance_weighted();
    double median();
    double minimum();
    double maximum();
    void updateFunc();

    OP m_op;
    std::function<double()> m_func;
    double m_inputNullValue;
    std::vector< std::pair<double,double>> m_data;
};

#endif // SMOOTHPROCESSOR_H
