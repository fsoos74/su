#ifndef MATHPROCESSOR_H
#define MATHPROCESSOR_H

#include <functional>
#include <QString>


class MathProcessor
{
public:

    enum class OP{ SET_V, SET_G1, SET_G2, ADD_GV, SUB_GV, MUL_GV, DIV_GV, POW_GV,DIV_VG, POW_VG,
                   ADD_GG, SUB_GG, MUL_GG, DIV_GG, ADD_MUL_GVG, NORM_GG, REL_DIFF_GG,
                   OVERLAY_G2_G1};

    static const double NULL_VALUE;

    MathProcessor();

    OP op()const{
        return m_op;
    }

    double input1()const{
        return m_input1;
    }

    double input2()const{
        return m_input2;
    }

    double value()const{
        return m_value;
    }

    double inputNullValue(){
        return m_inputNullValue;
    }

    void setOP(OP);
    void setInput1(const double&);
    void setInput2(const double&);
    void setValue(const double&);
    void setInputNullValue(const double&);

    double compute();

    static QStringList opList(QString inputName="Input");
    static QString toUserString(QString,QString inputName);
    static QString toFunctionString(QString, QString inputName);
    static QString toQString(OP, QString inputName="Input");
    static OP toOP(QString, QString inputName="Input");

private:

    double set_v();
    double set_g1();
    double set_g2();
    double add_gv();
    double sub_gv();
    double mul_gv();
    double div_gv();
    double pow_gv();
    double div_vg();
    double pow_vg();
    double add_gg();
    double sub_gg();
    double mul_gg();
    double div_gg();
    double add_mul_gvg();
    double norm_gg();
    double rel_diff_gg();
    double overlay_g2_g1();
    void updateFunc();

    OP m_op;
    std::function<double()> m_func;
    double m_input1;
    double m_input2;
    double m_value;
    double m_inputNullValue;
};

#endif // MATHPROCESSOR_H
