#ifndef WINDOWREDUCTIONFUNCTION
#define WINDOWREDUCTIONFUNCTION

#include <QMap>
#include <memory>
#include <trace.h>


enum class ReductionMethod { Sum, Mean, Minimum, Maximum, RMS };
static const QMap<ReductionMethod, QString > ReductionMethodsAndNames{

    { ReductionMethod::Sum, "Sum" },
    { ReductionMethod::Mean, "Mean" },
    { ReductionMethod::Minimum, "Minimum" },
    { ReductionMethod::Maximum, "Maximum" },
    { ReductionMethod::RMS, "RMS" }

};

ReductionMethod string2ReductionMethod( const QString& );
QString ReductionMethod2String( ReductionMethod );

struct ReductionFunction{
    virtual double operator()( seismic::Trace::Samples::const_iterator begin, seismic::Trace::Samples::const_iterator end )=0;
};


std::unique_ptr<ReductionFunction> reductionFunctionFactory( ReductionMethod);



#endif // WINDOWREDUCTIONFUNCTION

