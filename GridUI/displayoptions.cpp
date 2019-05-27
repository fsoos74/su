#include "displayoptions.h"

namespace sliceviewer{


DisplayOptions::DisplayOptions()
{

}

void DisplayOptions::setDisplayVolumes(bool on){
    mDisplayVolumes=on;
}

void DisplayOptions::setDisplayHorizons(bool on){
    mDisplayHorizons=on;
}

void DisplayOptions::setDisplayWells(bool on){
    mDisplayWells=on;
}

void DisplayOptions::setDisplayMarkers(bool on){
    mDisplayMarkers=on;
}

void DisplayOptions::setDisplayTables(bool on){
    mDisplayTables=on;
}

void DisplayOptions::setDisplayLastViewed(bool on){
    mDisplayLastViewed=on;
}

void DisplayOptions::setLastViewedColor(QColor color){
    mLastViewedColor=color;
}

void DisplayOptions::setWellVisibilityDistance(int d){
    mWellVisibilityDistance=d;
}

void DisplayOptions::setSharpen(bool on){
    mSharpen=on;
}

void DisplayOptions::setSharpenFilterSize(int s){
    mSharpenFilterSize=s;
}

void DisplayOptions::setSharpenFilterStrength(int p){
    mSharpenFilterStrength=p;
}

void DisplayOptions::setAntiAliasing(bool on){
    mAntiAliasing=on;
}

void DisplayOptions::setInlineOrientation(Qt::Orientation orientation){
    mInlineOrientation=orientation;
}

}
