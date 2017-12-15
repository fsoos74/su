#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:48:10
#
#-------------------------------------------------

QT       += widgets
QT += sql

//QT       -= gui

TARGET = GridUI
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG +=staticlib
}

DEFINES += GRIDUI_LIBRARY

SOURCES += gridui.cpp \
    gridview.cpp \
    displayrangedialog.cpp \
    isolinedialog.cpp \
    orientationdialog.cpp \
    gridviewer.cpp \
    griddisplayoptionsdialog.cpp \
    aspectratiodialog.cpp \
    scaledialog.cpp \
    griduiutil.cpp \
    gridselectdialog.cpp \
    volumeview.cpp \
    volumepropertydialog.cpp \
    volumeviewer2d.cpp \
    volumeitemsconfigdialog.cpp \
    playerdialog.cpp

HEADERS += gridui.h\
        gridui_global.h \
    gridview.h \
    displayrangedialog.h \
    isolinedialog.h \
    orientationdialog.h \
    gridviewer.h \
    griddisplayoptionsdialog.h \
    aspectratiodialog.h \
    scaledialog.h \
    griduiutil.h \
    gridselectdialog.h \
    volumeview.h \
    volumepropertydialog.h \
    volumeviewer2d.h \
    volumeitemsconfigdialog.h \
    playerdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}



FORMS += \
    displayrangedialog.ui \
    isolinedialog.ui \
    orientationdialog.ui \
    gridviewer.ui \
    griddisplayoptionsdialog.ui \
    aspectratiodialog.ui \
    scaledialog.ui \
    gridselectdialog.ui \
    volumepropertydialog.ui \
    volumeviewer2d.ui \
    playerdialog.ui



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilfs/release/ -lutilfs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilfs/debug/ -lutilfs
else:unix: LIBS += -L$$OUT_PWD/../utilfs/ -lutilfs

INCLUDEPATH += $$PWD/../utilfs
DEPENDPATH += $$PWD/../utilfs

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/release/ -lColorUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/debug/ -lColorUI
else:unix: LIBS += -L$$OUT_PWD/../ColorUI/ -lColorUI

INCLUDEPATH += $$PWD/../ColorUI
DEPENDPATH += $$PWD/../ColorUI

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilUI/release/ -lutilUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilUI/debug/ -lutilUI
else:unix: LIBS += -L$$OUT_PWD/../utilUI/ -lutilUI

INCLUDEPATH += $$PWD/../utilUI
DEPENDPATH += $$PWD/../utilUI

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

# added for intersection times
#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SeismicUI/release/ -lSeismicUI
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SeismicUI/debug/ -lSeismicUI
#else:unix: LIBS += -L$$OUT_PWD/../SeismicUI/ -lSeismicUI
#INCLUDEPATH += $$PWD/../SeismicUI
#DEPENDPATH += $$PWD/../SeismicUI

#added for intersection times
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Seismic/release/ -lSeismic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Seismic/debug/ -lSeismic
else:unix: LIBS += -L$$OUT_PWD/../Seismic/ -lSeismic

INCLUDEPATH += $$PWD/../Seismic
DEPENDPATH += $$PWD/../Seismic


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Well/release/ -lWell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Well/debug/ -lWell
else:unix: LIBS += -L$$OUT_PWD/../Well/ -lWell

INCLUDEPATH += $$PWD/../Well
DEPENDPATH += $$PWD/../Well
