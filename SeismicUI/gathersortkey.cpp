#include "gathersortkey.h"

#include<stdexcept>

const QString sortNoneStr("None");
const QString sortInlineStr("Inline");
const QString sortCrosslineStr("Crossline");
const QString sortOffsetStr("Offset");

QMap<GatherSortKey, QString> lookup{

    {GatherSortKey::None, sortNoneStr },
    {GatherSortKey::Inline, sortInlineStr },
    {GatherSortKey::Crossline, sortCrosslineStr },
    {GatherSortKey::Offset, sortOffsetStr }

};

QString toQString( const GatherSortKey& k){

    return lookup.value(k);
}

GatherSortKey toGatherSortKey(const QString& str){

    return lookup.key(str, GatherSortKey::None);
}


QDataStream& operator<<(QDataStream& out, const GatherSortKey& o){

    out<<toQString(o);

    return out;
}

QDataStream& operator>>(QDataStream& in, GatherSortKey& o){

    QString str;

    in>>str;

    o=toGatherSortKey(str);

    return in;
}

