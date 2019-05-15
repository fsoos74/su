#ifndef DISPLAYOPTIONS_H
#define DISPLAYOPTIONS_H

#include<QColor>

class DisplayOptions
{
public:
    DisplayOptions();

    bool isDisplayVolumes()const{
        return mDisplayVolumes;
    }

    bool isDisplayHorizons()const{
        return mDisplayHorizons;
    }

    bool isDisplayWells()const{
        return mDisplayWells;
    }

    bool isDisplayMarkers()const{
        return mDisplayMarkers;
    }

    bool isDisplayTables()const{
        return mDisplayTables;
    }

    bool isDisplayLastViewed()const{
        return mDisplayLastViewed;
    }

    QColor lastViewedColor()const{
        return mLastViewedColor;
    }

    int wellVisibilityDistance()const{
        return mWellVisibilityDistance;
    }

    bool isSharpen()const{
        return mSharpen;
    }

    int sharpenFilterSize()const{
        return mSharpenFilterSize;
    }

    int sharpenFilterStrength()const{
        return mSharpenFilterStrength;
    }

    void setDisplayVolumes(bool);
    void setDisplayHorizons(bool);
    void setDisplayWells(bool);
    void setDisplayMarkers(bool);
    void setDisplayTables(bool);
    void setDisplayLastViewed(bool);
    void setLastViewedColor(QColor);
    void setWellVisibilityDistance(int);
    void setSharpen(bool);
    void setSharpenFilterSize(int);
    void setSharpenFilterStrength(int);

private:
    bool mDisplayVolumes=true;
    bool mDisplayHorizons=true;
    bool mDisplayWells=true;
    bool mDisplayMarkers=true;
    bool mDisplayTables=true;
    bool mDisplayLastViewed=true;
    QColor mLastViewedColor=Qt::lightGray;
    int mWellVisibilityDistance=100;
    bool mSharpen=false;
    int mSharpenFilterSize=3;
    int mSharpenFilterStrength=50;
};

#endif // DISPLAYOPTIONS_H
