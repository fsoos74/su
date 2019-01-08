#include "volumedef.h"

VolumeDef::VolumeDef(std::shared_ptr<Volume> vol, ColorTable* ct)
    :m_volume(vol), m_colorTable(ct)
{
}

void VolumeDef::setVolume(std::shared_ptr<Volume> v){
    m_volume=v;
}

void VolumeDef::setColorTable(ColorTable * ct){
    m_colorTable=ct;
}
