#ifndef VOLUMEDEF_H
#define VOLUMEDEF_H

#include<volume.h>
#include<colortable.h>
#include<memory>

class VolumeDef
{
public:
    VolumeDef()=default;
    VolumeDef(std::shared_ptr<Volume>,ColorTable*);

    std::shared_ptr<Volume> volume()const{
        return m_volume;
    }

    ColorTable* colorTable()const{
        return m_colorTable;
    }

    void setVolume(std::shared_ptr<Volume>);
    void setColorTable(ColorTable*);

private:
    std::shared_ptr<Volume> m_volume;
    ColorTable* m_colorTable=nullptr;
};


inline bool operator==(const VolumeDef& v1, const VolumeDef& v2){
    return (v1.volume()==v2.volume()) && (v1.colorTable()==v2.colorTable());
}

#endif // VOLUMEDEF_H
