#-------------------------------------------------
#
# Project created by QtCreator 2015-11-07T19:22:39
#
#-------------------------------------------------


##XXX ADD FOR RELEASE, LICENSE
win32{
    DEFINES += USE_KEYLOCK_LICENSE
}

QT += core
QT += gui
QT += printsupport
QT += widgets   # needed for QTransform in projectgeometry
QT += sql
QT += script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../common.pri)


CONFIG += qtc_runnable

TARGET = ProjectViewer
TEMPLATE = app


SOURCES += main.cpp\
    addwelldialog.cpp \
        projectviewer.cpp \
    seismicdataselector.cpp \
    datasetpropertiesdialog.cpp \
    selectgridtypeandnamedialog.cpp \
    twocombosdialog.cpp \
    amplitudecurvedataselectiondialog.cpp \
    curveviewerdisplayoptionsdialog.cpp \
    amplitudecurveviewer.cpp \
    projectgeometrydialog.cpp \
    aboutdialog.cpp \
    seismicsliceselector.cpp \
    volumesliceselector.cpp \
    volumerelativesliceselector.cpp \
    compasswidget.cpp \
    gridcolorcompositeinputdialog.cpp \
    colorcompositeviewer.cpp \
    spectrumviewer.cpp \
    spectrumdataselectiondialog.cpp \
    processparamsviewer.cpp \
    logsitemdelegate.cpp \
    tabledialog.cpp

HEADERS  += projectviewer.h \
    addwelldialog.h \
    seismicdataselector.h \
    datasetpropertiesdialog.h \
    selectgridtypeandnamedialog.h \
    twocombosdialog.h \
    amplitudecurvedataselectiondialog.h \
    curveviewerdisplayoptionsdialog.h \
    amplitudecurveviewer.h \
    projectgeometrydialog.h \
    aboutdialog.h \
    amplitudecurvedefinition.h \
    licenseinfo.h \
    seismicsliceselector.h \
    volumesliceselector.h \
    volumerelativesliceselector.h \
    compasswidget.h \
    gridcolorcompositeinputdialog.h \
    colorcompositeviewer.h \
    spectrumviewer.h \
    spectrumdataselectiondialog.h \
    spectrumdefinition.h \
    processparamsviewer.h \
    logsitemdelegate.h \
    tabledialog.h

FORMS    += projectviewer.ui \
    addwelldialog.ui \
    seismicdataselector.ui \
    datasetpropertiesdialog.ui \
    selectgridtypeandnamedialog.ui \
    twocombosdialog.ui \
    amplitudecurvedataselectiondialog.ui \
    curveviewerdisplayoptionsdialog.ui \
    amplitudecurveviewer.ui \
    projectgeometrydialog.ui \
    aboutdialog.ui \
    seismicsliceselector.ui \
    volumesliceselector.ui \
    volumerelativesliceselector.ui \
    gridcolorcompositeinputdialog.ui \
    colorcompositeviewer.ui \
    spectrumviewer.ui \
    spectrumdataselectiondialog.ui \
    processparamsviewer.ui \
    tabledialog.ui

RESOURCES += \
    icons.qrc


# utils
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilfs/release/ -lutilfs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilfs/debug/ -lutilfs
else:unix: LIBS += -L$$OUT_PWD/../utilfs/ -lutilfs

INCLUDEPATH += $$PWD/../utilfs
DEPENDPATH += $$PWD/../utilfs


# database
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Database/release/ -lDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Database/debug/ -lDatabase
else:unix: LIBS += -L$$OUT_PWD/../Database/ -lDatabase

INCLUDEPATH += $$PWD/../Database
DEPENDPATH += $$PWD/../Database


#colorui
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/release/ -lColorUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/debug/ -lColorUI
else:unix: LIBS += -L$$OUT_PWD/../ColorUI/ -lColorUI

INCLUDEPATH += $$PWD/../ColorUI
DEPENDPATH += $$PWD/../ColorUI


#project
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project


#databaseui
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DatabaseUI/release/ -lDatabaseUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DatabaseUI/debug/ -lDatabaseUI
else:unix: LIBS += -L$$OUT_PWD/../DatabaseUI/ -lDatabaseUI

INCLUDEPATH += $$PWD/../DatabaseUI
DEPENDPATH += $$PWD/../DatabaseUI


#seismicui
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SeismicUI/release/ -lSeismicUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SeismicUI/debug/ -lSeismicUI
else:unix: LIBS += -L$$OUT_PWD/../SeismicUI/ -lSeismicUI

INCLUDEPATH += $$PWD/../SeismicUI
DEPENDPATH += $$PWD/../SeismicUI


#wellui
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../WellUI/release/ -lWellUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../WellUI/debug/ -lWellUI
else:unix: LIBS += -L$$OUT_PWD/../WellUI/ -lWellUI

INCLUDEPATH += $$PWD/../WellUI
DEPENDPATH += $$PWD/../WellUI


#gridui
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GridUI/release/ -lGridUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GridUI/debug/ -lGridUI
else:unix: LIBS += -L$$OUT_PWD/../GridUI/ -lGridUI

INCLUDEPATH += $$PWD/../GridUI
DEPENDPATH += $$PWD/../GridUI

#sliceviewer
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibSliceViewer/release/ -lLibSliceViewer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibSliceViewer/debug/ -lLibSliceViewer
else:unix: LIBS += -L$$OUT_PWD/../LibSliceViewer/ -lLibSliceViewer

INCLUDEPATH += $$PWD/../LibSliceViewer
DEPENDPATH += $$PWD/../LibSliceViewer

#logviewer  (NEW)
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibLogViewer/release/ -lLibLogViewer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibLogViewer/debug/ -lLibLogViewer
else:unix: LIBS += -L$$OUT_PWD/../LibLogViewer/ -lLibLogViewer

INCLUDEPATH += $$PWD/../LibLogViewer
DEPENDPATH += $$PWD/../LibLogViewer



#crossplotviewer
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../CrossplotViewer/release/ -lCrossplotViewer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../CrossplotViewer/debug/ -lCrossplotViewer
else:unix: LIBS += -L$$OUT_PWD/../CrossplotViewer/ -lCrossplotViewer

INCLUDEPATH += $$PWD/../CrossplotViewer
DEPENDPATH += $$PWD/../CrossplotViewer


#scatterplotviewer
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ScatterplotViewer/release/ -lScatterplotViewer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ScatterplotViewer/debug/ -lScatterplotViewer
else:unix: LIBS += -L$$OUT_PWD/../ScatterplotViewer/ -lScatterplotViewer

INCLUDEPATH += $$PWD/../ScatterplotViewer
DEPENDPATH += $$PWD/../ScatterplotViewer


#viewer2d
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Viewer2D/release/ -lViewer2D
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Viewer2D/debug/ -lViewer2D
else:unix: LIBS += -L$$OUT_PWD/../Viewer2D/ -lViewer2D

INCLUDEPATH += $$PWD/../Viewer2D
DEPENDPATH += $$PWD/../Viewer2D


#viewer3d
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Viewer3D/release/ -lViewer3D
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Viewer3D/debug/ -lViewer3D
else:unix: LIBS += -L$$OUT_PWD/../Viewer3D/ -lViewer3D

INCLUDEPATH += $$PWD/../Viewer3D
DEPENDPATH += $$PWD/../Viewer3D


#statistics
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Statistics/release/ -lStatistics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Statistics/debug/ -lStatistics
else:unix: LIBS += -L$$OUT_PWD/../Statistics/ -lStatistics

INCLUDEPATH += $$PWD/../Statistics
DEPENDPATH += $$PWD/../Statistics


#statisticsui
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StatisticsUI/release/ -lStatisticsUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StatisticsUI/debug/ -lStatisticsUI
else:unix: LIBS += -L$$OUT_PWD/../StatisticsUI/ -lStatisticsUI

INCLUDEPATH += $$PWD/../StatisticsUI
DEPENDPATH += $$PWD/../StatisticsUI


#well
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Well/release/ -lWell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Well/debug/ -lWell
else:unix: LIBS += -L$$OUT_PWD/../Well/ -lWell

INCLUDEPATH += $$PWD/../Well
DEPENDPATH += $$PWD/../Well


#seismic
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Seismic/release/ -lSeismic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Seismic/debug/ -lSeismic
else:unix: LIBS += -L$$OUT_PWD/../Seismic/ -lSeismic

INCLUDEPATH += $$PWD/../Seismic
DEPENDPATH += $$PWD/../Seismic


#grid
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Grid/release/ -lGrid
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Grid/debug/ -lGrid
else:unix: LIBS += -L$$OUT_PWD/../Grid/ -lGrid

INCLUDEPATH += $$PWD/../Grid
DEPENDPATH += $$PWD/../Grid


#project AGAIN - FIX need only 1
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project


#prozess
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../prozess/release/ -lprozess
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../prozess/debug/ -lprozess
else:unix: LIBS += -L$$OUT_PWD/../prozess/ -lprozess

INCLUDEPATH += $$PWD/../prozess
DEPENDPATH += $$PWD/../prozess


#prozessui
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ProzessUI/release/ -lProzessUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ProzessUI/debug/ -lProzessUI
else:unix: LIBS += -L$$OUT_PWD/../ProzessUI/ -lProzessUI

INCLUDEPATH += $$PWD/../ProzessUI
DEPENDPATH += $$PWD/../ProzessUI


#utilui
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilUI/release/ -lutilUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilUI/debug/ -lutilUI
else:unix: LIBS += -L$$OUT_PWD/../utilUI/ -lutilUI

INCLUDEPATH += $$PWD/../utilUI
DEPENDPATH += $$PWD/../utilUI


#crypt
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Crypt/release/ -lCrypt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Crypt/debug/ -lCrypt
else:unix: LIBS += -L$$OUT_PWD/../Crypt/ -lCrypt

INCLUDEPATH += $$PWD/../Crypt
DEPENDPATH += $$PWD/../Crypt



# additional libraries for use of dongle:
win32{
  LIBS += -lwsock32
  LIBS += -liphlpapi
  LIBS += -lnetapi32
  LIBS += -luser32
  LIBS += -ladvapi32
  LIBS += -lshell32

  #this is for msvc2013 on dell
  #LIBS += -L$$PWD/../../../Keylok/API/Windows/VS2013/ -lkfunc64MD
  #LIBS += -L$$PWD/../../build-dongle-Desktop_Qt_5_5_1_MSVC2013_64bit-Release/release/ -ldongle

  #this is for msvc2017 on hp
  LIBS += -L$$PWD/../../../Keylok/API/Windows/VS2015/ -lkfunc64MD       # binary compatible
  LIBS += -L$$PWD/../../build-dongle-Desktop_Qt_5_11_1_MSVC2017_64bit-Release/release/ -ldongle

  INCLUDEPATH += $$PWD/../../dongle
  DEPENDPATH += $$PWD/../../dongle
}

DISTFILES += \
    ../images/select-range-icon.png \
    ../images/current-point-icon-32x32.png \
    ../images/pause-32x32.png \
    ../images/play-32x32.png \
    ../images/stop-32x32.png \
    ../images/crossplot-volumes-32x32.png \
    ../images/crossplot-logs-32x32.png \
    ../images/crossplot-grids-32x32.png \
    ../images/minus_16x16.png \
    ../images/plus_16x16.png \
    ../images/add-by-well-32x32.png \
    ../images/add-by-log-32x32.png \
    ../images/pick-outline-32x32.png \
    ../images/setup-wells-32x32.png \
    ../images/setup-slices-32x32.png \
    ../images/setup-horizons-32x32.png

