#include "las_reader.h"

#include<iostream>
#include<fstream>
#include<sstream>
#include<regex>
#include<list>
#include<unordered_map>
#include<cmath>

#include<QApplication>

namespace well{

LASReader::LASReader( LASData& data, QObject* parent ) : QObject(parent), data(data){
}

void LASReader::cancel(){

    m_canceled=true;
}


bool LASReader::read( std::ifstream& fin){

    m_canceled=false;

    //clear old data if exists
    data=LASData();

    fin.seekg(0L, std::ios::end);
    int n=static_cast<int>(fin.tellg());
    fin.seekg(0L, std::ios::beg);

    emit started(n);
    qApp->processEvents();

    while( fin.good() && !m_canceled){

        std::string line;

        if( !std::getline( fin, line ) )break;
        linesRead++;

        if( (linesRead % 1000) == 0 ){
            int n=static_cast<int>(fin.tellg());
            emit progress(n);           
        }
        qApp->processEvents();


        // use only allow ascii codes
        cleanLine( line );

        // ignore comments
        if( line[0]=='#' ) continue;

        // new section
        if( line[0]=='~' ){

            stopSection(currentSectionCode);

            startSection(line[1] );

            continue;
        }

        // actual data
        if( currentSectionCode=='A' ){
            if(!processDataLine(line)) return false;
            continue;
        }

        // other information is plain text
        if( currentSectionCode=='O' ){
            data.otherInformation.push_back(line);
            continue;
        }

        LASMnemonic mnem;
        if( !processSectionLine(line, mnem) ){
            setError("Invalid line in section!");
            return false;
        }

        switch( currentSectionCode ){

        case 'V': data.versionInformation[mnem.name]=mnem; break;
        case 'W': data.wellInformation[mnem.name]=mnem; break;
        case 'P': data.parameterInformation[mnem.name]=mnem; break;
        case 'C': data.curveInformation.push_back(mnem);break;
        default: throw std::runtime_error("Invalid section code but can never reach here!");break;
        }
    }

    if( m_canceled){
        setError("Canceled by user!");
        return false;
    }

    emit finished();
    qApp->processEvents();

    return true;
}


// las file may only contain ascii 10,13,32-126
void LASReader::cleanLine(std::string& line){

    for( auto &c: line){

        if( ( c<32 && c!=10 && c!=13 ) || ( c>126 ) ){
            c=' ';
        }
    }
}

bool LASReader::processSectionLine( const std::string& line, LASMnemonic& mnem ){

    //char c=line.back();
    static std::regex rgx(R"(^\s*([^ .:]+)\s*\.([^ .:]*)\s+(.*)\s*:\s*(.*)\s*(\r\n|\n|\r)?$)");
    std::smatch match;
    if(!regex_match(line, match, rgx) ){
        return false;
    }
    //std::cout<<match.size()<<" : "<<line<<std::endl<<std::flush;
    if(line.size()<5) return false;
    mnem=LASMnemonic( match[1], match[2], match[3], match[4]);

    return true;
}

bool LASReader::startSection( char code ){

    if( (code=='V' && !data.versionInformation.empty() ) ||
        ( code=='W' && !data.wellInformation.empty() ) ||
        ( code=='C' && !data.curveInformation.empty() ) ){

        setError("LAS specification allows only one 'V', 'W', 'C' section!");
        return false;
    }

    currentSectionCode=code;

    return true;
}

bool LASReader::stopSection( char code ){
    switch( code ){

        case 'V': return processVersionInformation();break;
        case 'W': return processWellInformation();break;
        case 'C': return processCurveInformation(); break;
        default: break;
    }

    return true;
}

bool LASReader::getDoubleMnemonic( std::unordered_map<std::string,LASMnemonic>& s, const std::string& mnem, double& data){

    std::unordered_map<std::string, LASMnemonic>::iterator it;
    it=s.find(mnem);
    if( it==s.end()){
        return false;
    }
    data=std::atof( it->second.data.c_str());
    return true;
}


bool LASReader::processVersionInformation(){

    std::unordered_map<std::string, LASMnemonic>::iterator it;
    it=data.versionInformation.find("VERS");
    if( it==data.versionInformation.end()){
        setError("Section V has no VERS LASMnemonic!");
        return false;
    }
    if( !regex_match(it->second.data, std::regex(R"(^\s*2\.0.*)" )  ) ){
        setError("Wrong LAS version!");
        return false;
    }

    it=data.versionInformation.find("WRAP");
    if( it==data.versionInformation.end()){
        setError("Section V has no WRAP LASMnemonic!");
        return false;
    }
    if( regex_match(it->second.data, std::regex(R"(^\s*(yes|YES).*)" ) ) ){
        wrap=true;
    }
    else if( regex_match(it->second.data, std::regex(R"(^\s*(no|NO).*)" ) ) ){
        wrap=false;
    }
    else{
        setError("Illegal wrap setting!");
        return false;
    }

    return true;
}


bool LASReader::processWellInformation(){

    if( !getDoubleMnemonic( data.wellInformation, "STRT", start ) ){
        setError("Section W has no STRT!");
        return false;
    }

    if( !getDoubleMnemonic( data.wellInformation, "STOP", stop ) ){
        setError("Section W has no STOP!");
        return false;
    }

    if( !getDoubleMnemonic( data.wellInformation, "STEP", step ) ){
        setError("Section W has no STEP!");
        return false;
    }

    if( (stop-start)*step <=0 ){
        setError("Invalid range (STRT,STOP,STEP) in section W!");
        return false;
    }

    if( !getDoubleMnemonic( data.wellInformation, "NULL", lasNULLValue ) ){
        setError("Section W has no NULL value definition!");
        return false;
    }

    return true;
}

bool LASReader::processCurveInformation(){

    if( data.curveInformation.empty()){
        setError("No curves defined!");
        return false;
    }

    // TODO check first curve type depth,time


    // compute number of samples
    int n=std::round( ( stop - start )/step) +1;


    for( auto curveInfo: data.curveInformation ){

        data.curves.push_back( std::vector<double>(n, std::numeric_limits<double>::max()) );
    }

    return true;
}


bool LASReader::processDataLineWrap( const std::string& line){

    if( curveIndex==data.curves.size()){
         sampleIndex++;
         curveIndex=0;
    }

    if( sampleIndex>=data.curves.front().size() ){
        //std::cerr<<"!!!"<<sampleIndex<<" / "<<data.curves.front().size()<<std::endl;
        setError("Excessive curve samples!");
        return false;
    }

    std::istringstream iss(line);
    double v;

    while( iss>>v ){

        if( curveIndex == data.curves.size() ){
            setError("Start of new step within line!");
            return false;
        }

        if( v==lasNULLValue){
            v=std::numeric_limits<double>::max();
        }
        data.curves[curveIndex][sampleIndex]=v;
        curveIndex++;
    }

    return true;
}

bool LASReader::processDataLineLong( const std::string& line){

    if( sampleIndex>=data.curves.front().size() ){
        setError("Excessive curve samples!");
        return false;
    }

    std::istringstream iss(line);
    curveIndex=0;
    double v;
    while( iss>>v ){

        if( curveIndex==data.curves.size() ){
            setError("Excessive samples!");
            return false;
        }

        if( v==lasNULLValue){
            v=std::numeric_limits<double>::max();
        }

        data.curves[curveIndex][sampleIndex]=v;

        curveIndex++;
    }

    if( curveIndex<data.curves.size()){
        setError("Run out of samples!");
        return false;
    }

    sampleIndex++;

    return true;
}


bool LASReader::processDataLine( const std::string& line){

    if( wrap ){

        return processDataLineWrap(line);
    }
    else{

        return processDataLineLong(line);
    }
}



void LASReader::setError( const std::string& msg ){

    error=true;
    errorMsg=msg;
}


} // namespace las

