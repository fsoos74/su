#ifndef INTEGRATIONPROCESSOR_H
#define INTEGRATIONPROCESSOR_H

#include <functional>
#include <QString>


class IntegrationProcessor
{
public:

    enum class OP{ SUM, AVG };

    static const double NULL_VALUE;

    IntegrationProcessor();

    OP op()const{
        return m_op;
    }

    double input()const{
        return m_input;
    }

    double inputNullValue(){
        return m_inputNullValue;
    }

    void setOP(OP);
    void setInputNullValue(const double&);

    void restart(const double& initial=0);
    void addValue(const double&);
    double compute();

    static QStringList opList();
    static QString toQString(OP);
    static OP toOP(QString);

private:

    double sum();
    double avg();

    void updateFunc();

    OP m_op;
    std::function<double()> m_func;
    double m_input;
    double m_sum=0;
    size_t m_count=0;
    double m_inputNullValue;
};

#endif // MATHPROCESSOR_H
