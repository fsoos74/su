#ifndef LAS_READER_H
#define LAS_READER_H

#include "las_mnemonic.h"
#include "las_data.h"

#include<QObject>

namespace well{

class LASReader : public QObject{

    Q_OBJECT

public:

    LASReader( LASData& data, QObject* parent=0 );

    bool read( std::ifstream& fin);

    bool hasError()const{
        return error;
    }

    std::string lastError()const{
        return errorMsg;
    }

signals:

    void started( int nsteps);
    void finished();
    void progress(int);
    //void error(QString);

public slots:

    void cancel();

private:

    int currentLine()const{
        return linesRead;
    }

    // las file may only contain ascii 10,13,32-126
    void cleanLine(std::string& line);

    bool processSectionLine( const std::string& line, LASMnemonic& mnem );

    bool startSection( char code );

    bool stopSection( char code );

    bool getDoubleMnemonic( std::unordered_map<std::string,LASMnemonic>& s, const std::string& mnem, double& data);

    bool processVersionInformation();

    bool processWellInformation();

    bool processCurveInformation();

    bool processDataLine( const std::string& line);
    bool processDataLineWrap( const std::string& line);
    bool processDataLineLong( const std::string& line);

    void setError( const std::string& msg );


    LASData& data;

    char currentSectionCode='0';
    int  linesRead=0;
    double start;
    double stop;
    double step;
    double lasNULLValue;
    size_t curveIndex=0;
    size_t sampleIndex=0;
    bool wrap=false;
    bool error=false;
    std::string errorMsg;

    bool m_canceled=false;

};  // Reader


} // namespace las


#endif // READER_H
