#include "utilities.h"

#include<segyinfo.h>
#include<segy_header_def.h>
#include<segyreader.h>

#include<QStringList>

#include<vector>

void setRequiredHeaderwords( seismic::SEGYReader& reader, const QStringList& words){

    seismic::SEGYInfo info=reader.info();
    const std::vector<seismic::SEGYHeaderWordDef> allHeaderWords=info.traceHeaderDef();
    std::vector<seismic::SEGYHeaderWordDef> usedHeaderWords;
    for( seismic::SEGYHeaderWordDef def : allHeaderWords ){
        if( words.contains(def.name.c_str()) ){
            usedHeaderWords.push_back(def);
        }
    }
    info.setTraceHeaderDef(usedHeaderWords);
    reader.setInfo(info);


}
