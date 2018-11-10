#ifndef LOG_H
#define LOG_H

#include<vector>
#include<limits>
#include<algorithm>
#include<string>

// use int for index to allow easier computations, use
class Log
{

public:    

    typedef std::vector<double>::iterator iterator;
    typedef std::vector<double>::const_iterator const_iterator;

    Log():m_name("generic"),m_unit("NA"),m_descr(""),m_z0(0), m_dz(0){

    }

    //Log(const Log&)=default;

    Log( const std::string& name, const std::string& unit, const std::string& descr, const double& z0, const double& dz, int nz,
         const double& ival=NULL_VALUE):        // ival was 0, 9.3.18
        m_name(name), m_unit(unit), m_descr(descr), m_z0(z0), m_dz(dz), m_data( nz,ival ){
    }

    Log( const std::string& name, const std::string& unit, const std::string& descr, const double& z0, const double& dz, std::vector<double> dat) :
        m_name(name), m_unit(unit), m_descr(descr), m_z0(z0), m_dz(dz), m_data( dat ){
    }

    template<class ITER>
    Log( const std::string& name, const std::string& unit, const std::string& descr, const double& z0, const double& dz,
         ITER beg, ITER end, const double& null_value=std::numeric_limits<double>::max() )
        : m_name(name), m_unit(unit), m_descr(descr), m_z0(z0), m_dz(dz), m_data(beg, end){
        std::replace( m_data.begin(), m_data.end(), null_value, NULL_VALUE );
    }

    std::string name()const{
        return m_name;
    }

    std::string descr()const{
        return m_descr;
    }

    std::string unit()const{
        return m_unit;
    }

    double z0()const{
        return m_z0;
    }

    double dz()const{
        return m_dz;
    }

    const std::vector<double>& data()const{
        return m_data;
    }

    int nz()const{
        return static_cast<int>(m_data.size());
    }

    double lz()const{
        return (nz()>0 ) ? index2z(nz()-1) : m_z0;
    }

    bool empty()const{
        return m_data.empty();
    }

    iterator begin(){
        return m_data.begin();
    }

    iterator end(){
        return m_data.end();
    }

    const_iterator cbegin()const{
        return m_data.cbegin();
    }

    const_iterator cend()const{
        return m_data.cend();
    }

    double operator[]( int i)const{
        return m_data[i];
    }

    double& operator[]( int i){
        return m_data[i];
    }

    double at( int i)const{
        return m_data.at(i);
    }

    double& at( int i){
        return m_data.at(i);
    }

    // interpolate
    double operator()(double z){
        double id=(z-m_z0)/m_dz;
        int ii=static_cast<int>(id);
        if( ii<0 || ii+1>=nz() ) return NULL_VALUE;
        auto val0=m_data[ii];
        auto val1=m_data[ii+1];
        if( val0==NULL_VALUE || val1==NULL_VALUE) return NULL_VALUE;
        double fac=id-ii;
        return (1.-fac)*val0 + fac*val1;
    }

    int z2index(double z)const{
        return (z-m_z0)/m_dz;
    }

    double index2z(int i)const{
        return m_z0+i*m_dz;
    }

    void setName(const std::string&);
    void setUnit(const std::string&);
    void setDescr(const std::string&);
    void setZ0(double);
    void setDZ(double);

    std::pair<double,double> range()const;

    static double NULL_VALUE;

private:

    std::string m_name;
    std::string m_unit;
    std::string m_descr;
    double m_z0;
    double m_dz;
    std::vector<double> m_data;
};

#endif // LOG_H
