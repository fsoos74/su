#ifndef SELECTIONPOINT_H
#define SELECTIONPOINT_H


struct SelectionPoint
{
public:
    SelectionPoint(int il=0, int xl=0, double t=0);

    bool operator==(const SelectionPoint& other )const{
        return (iline==other.iline) && (xline==other.xline) && (time==other.time);
    }

    int iline;
    int xline;
    double time;
};


#endif // SELECTIONPOINT_H
