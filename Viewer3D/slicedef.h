#ifndef SLICEDEF_H
#define SLICEDEF_H

#include<QString>

enum class SliceType{ INLINE, CROSSLINE, SAMPLE };

QString toQString(SliceType);
SliceType toSliceType(QString);

struct SliceDef{
    SliceType type;
    int       value;
};

inline bool operator==(const SliceDef& s1, const SliceDef& s2){

    return ( s1.type == s2.type ) && (s1.value==s2.value);
}

#endif // SLICEDEF_H
