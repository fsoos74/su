#include "selectionpoint.h"

#include<limits>

const double SelectionPoint::NO_TIME=std::numeric_limits<double>::lowest();
const int SelectionPoint::NO_LINE=std::numeric_limits<int>::min();
const SelectionPoint SelectionPoint::NONE=SelectionPoint( SelectionPoint::NO_LINE, SelectionPoint::NO_LINE, SelectionPoint::NO_TIME);


SelectionPoint::SelectionPoint(int il, int xl, double t):
    iline(il), xline(xl), time(t){

}

