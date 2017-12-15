#-------------------------------------------------
#
# Project created by QtCreator 2015-11-07T19:22:39
#
#-------------------------------------------------



#XXX ADD FOR RELEASE, LICENSE
win32{
    DEFINES += USE_KEYLOCK_LICENSE
}

#embed python
unix {
    QMAKE_CFLAGS += `/usr/bin/python2.7-config --cflags`;
    #!!!! IMPORTANT: ADD path to libutil to avoid conflict with my libutil!!!!
    QMAKE_LFLAGS += -L/usr/lib64 `/usr/bin/python2.7-config --ldflags`
   # INCLUDEPATH +=/usr/include/python2.7
    #LIBS += -L/usr/lib64 -libutil.so
    #LIBS += -lpthread -ldl -lutil -lm -Xlinker -export-dynamic
}
win32{
    LIBS += -L"C:\Python27\libs" -lpython27

}

QT       += core gui
QT += printsupport
QT += widgets   # needed for QTransform in projectgeometry

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../common.pri)

QT += sql
CONFIG += qtc_runnable

TARGET = ProjectViewer
TEMPLATE = app


SOURCES += main.cpp\
        projectviewer.cpp \
    seismicdataselector.cpp \
    datasetpropertiesdialog.cpp \
    selectgridtypeandnamedialog.cpp \
    twocombosdialog.cpp \
    crossplotviewer.cpp \
    amplitudecurvedataselectiondialog.cpp \
    curveviewerdisplayoptionsdialog.cpp \
    crossplotviewerdisplayoptionsdialog.cpp \
    amplitudecurveviewer.cpp \
    projectgeometrydialog.cpp \
    aboutdialog.cpp \
    crossplot.cpp \
    crossplotgridsinputdialog.cpp \
    crossplotvolumesinputdialog.cpp \
    edittrenddialog.cpp \
    seismicsliceselector.cpp \
    volumesliceselector.cpp \
    volumerelativesliceselector.cpp \
    compasswidget.cpp \
    mapviewer.cpp \
    gridcolorcompositeinputdialog.cpp \
    colorcompositeviewer.cpp \
    spectrumviewer.cpp \
    spectrumdataselectiondialog.cpp \
    processparamsviewer.cpp \
    logsitemdelegate.cpp \
    crossplotview.cpp

HEADERS  += projectviewer.h \
    seismicdataselector.h \
    datasetpropertiesdialog.h \
    selectgridtypeandnamedialog.h \
    twocombosdialog.h \
    crossplotviewer.h \
    amplitudecurvedataselectiondialog.h \
    curveviewerdisplayoptionsdialog.h \
    crossplotviewerdisplayoptionsdialog.h \
    amplitudecurveviewer.h \
    projectgeometrydialog.h \
    aboutdialog.h \
    amplitudecurvedefinition.h \
    licenseinfo.h \
    crossplot.h \
    crossplotgridsinputdialog.h \
    crossplotvolumesinputdialog.h \
    edittrenddialog.h \
    seismicsliceselector.h \
    volumesliceselector.h \
    volumerelativesliceselector.h \
    compasswidget.h \
    mapviewer.h \
    gridcolorcompositeinputdialog.h \
    colorcompositeviewer.h \
    spectrumviewer.h \
    spectrumdataselectiondialog.h \
    spectrumdefinition.h \
    processparamsviewer.h \
    logsitemdelegate.h \
    crossplotview.h

FORMS    += projectviewer.ui \
    seismicdataselector.ui \
    datasetpropertiesdialog.ui \
    selectgridtypeandnamedialog.ui \
    twocombosdialog.ui \
    crossplotviewer.ui \
    amplitudecurvedataselectiondialog.ui \
    curveviewerdisplayoptionsdialog.ui \
    crossplotviewerdisplayoptionsdialog.ui \
    amplitudecurveviewer.ui \
    projectgeometrydialog.ui \
    aboutdialog.ui \
    crossplotgridsinputdialog.ui \
    crossplotvolumesinputdialog.ui \
    edittrenddialog.ui \
    seismicsliceselector.ui \
    volumesliceselector.ui \
    volumerelativesliceselector.ui \
    mapviewer.ui \
    gridcolorcompositeinputdialog.ui \
    colorcompositeviewer.ui \
    spectrumviewer.ui \
    spectrumdataselectiondialog.ui \
    processparamsviewer.ui

RESOURCES += \
    icons.qrc



#maybe order matters, try put this after all libs that need it
#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
#else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil
#INCLUDEPATH += $$PWD/../util
#DEPENDPATH += $$PWD/../util

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilfs/release/ -lutilfs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilfs/debug/ -lutilfs
else:unix: LIBS += -L$$OUT_PWD/../utilfs/ -lutilfs

INCLUDEPATH += $$PWD/../utilfs
DEPENDPATH += $$PWD/../utilfs


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Database/release/ -lDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Database/debug/ -lDatabase
else:unix: LIBS += -L$$OUT_PWD/../Database/ -lDatabase

INCLUDEPATH += $$PWD/../Database
DEPENDPATH += $$PWD/../Database

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/release/ -lColorUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/debug/ -lColorUI
else:unix: LIBS += -L$$OUT_PWD/../ColorUI/ -lColorUI

INCLUDEPATH += $$PWD/../ColorUI
DEPENDPATH += $$PWD/../ColorUI

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DatabaseUI/release/ -lDatabaseUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DatabaseUI/debug/ -lDatabaseUI
else:unix: LIBS += -L$$OUT_PWD/../DatabaseUI/ -lDatabaseUI

INCLUDEPATH += $$PWD/../DatabaseUI
DEPENDPATH += $$PWD/../DatabaseUI

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SeismicUI/release/ -lSeismicUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SeismicUI/debug/ -lSeismicUI
else:unix: LIBS += -L$$OUT_PWD/../SeismicUI/ -lSeismicUI

INCLUDEPATH += $$PWD/../SeismicUI
DEPENDPATH += $$PWD/../SeismicUI


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../WellUI/release/ -lWellUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../WellUI/debug/ -lWellUI
else:unix: LIBS += -L$$OUT_PWD/../WellUI/ -lWellUI

INCLUDEPATH += $$PWD/../WellUI
DEPENDPATH += $$PWD/../WellUI


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GridUI/release/ -lGridUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GridUI/debug/ -lGridUI
else:unix: LIBS += -L$$OUT_PWD/../GridUI/ -lGridUI

INCLUDEPATH += $$PWD/../GridUI
DEPENDPATH += $$PWD/../GridUI


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Viewer2D/release/ -lViewer2D
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Viewer2D/debug/ -lViewer2D
else:unix: LIBS += -L$$OUT_PWD/../Viewer2D/ -lViewer2D

INCLUDEPATH += $$PWD/../Viewer2D
DEPENDPATH += $$PWD/../Viewer2D


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Viewer3D/release/ -lViewer3D
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Viewer3D/debug/ -lViewer3D
else:unix: LIBS += -L$$OUT_PWD/../Viewer3D/ -lViewer3D

INCLUDEPATH += $$PWD/../Viewer3D
DEPENDPATH += $$PWD/../Viewer3D

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Statistics/release/ -lStatistics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Statistics/debug/ -lStatistics
else:unix: LIBS += -L$$OUT_PWD/../Statistics/ -lStatistics

INCLUDEPATH += $$PWD/../Statistics
DEPENDPATH += $$PWD/../Statistics


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StatisticsUI/release/ -lStatisticsUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StatisticsUI/debug/ -lStatisticsUI
else:unix: LIBS += -L$$OUT_PWD/../StatisticsUI/ -lStatisticsUI

INCLUDEPATH += $$PWD/../StatisticsUI
DEPENDPATH += $$PWD/../StatisticsUI


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Well/release/ -lWell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Well/debug/ -lWell
else:unix: LIBS += -L$$OUT_PWD/../Well/ -lWell

INCLUDEPATH += $$PWD/../Well
DEPENDPATH += $$PWD/../Well


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Seismic/release/ -lSeismic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Seismic/debug/ -lSeismic
else:unix: LIBS += -L$$OUT_PWD/../Seismic/ -lSeismic

INCLUDEPATH += $$PWD/../Seismic
DEPENDPATH += $$PWD/../Seismic

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Grid/release/ -lGrid
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Grid/debug/ -lGrid
else:unix: LIBS += -L$$OUT_PWD/../Grid/ -lGrid

INCLUDEPATH += $$PWD/../Grid
DEPENDPATH += $$PWD/../Grid

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../prozess/release/ -lprozess
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../prozess/debug/ -lprozess
else:unix: LIBS += -L$$OUT_PWD/../prozess/ -lprozess

INCLUDEPATH += $$PWD/../prozess
DEPENDPATH += $$PWD/../prozess

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ProzessUI/release/ -lProzessUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ProzessUI/debug/ -lProzessUI
else:unix: LIBS += -L$$OUT_PWD/../ProzessUI/ -lProzessUI

INCLUDEPATH += $$PWD/../ProzessUI
DEPENDPATH += $$PWD/../ProzessUI

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilUI/release/ -lutilUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilUI/debug/ -lutilUI
else:unix: LIBS += -L$$OUT_PWD/../utilUI/ -lutilUI

INCLUDEPATH += $$PWD/../utilUI
DEPENDPATH += $$PWD/../utilUI



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
  LIBS += -L$$PWD/../../../Keylok/API/Windows/VS2013/ -lkfunc64MD
  LIBS += -L$$PWD/../../build-dongle-Desktop_Qt_5_5_1_MSVC2013_64bit-Release/release/ -ldongle

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
    ../images/crossplot-grids-32x32.png

