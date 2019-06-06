#-------------------------------------------------
#
# Project created by QtCreator 2019-06-06T10:46:34
#
#-------------------------------------------------

QT       += widgets
QT       += sql
//QT       -= gui

TARGET = LibSliceViewer
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG +=staticlib
}

DEFINES += LIBSLICEVIEWER_LIBRARY


SOURCES += \
    libsliceviewer.cpp \
    compasswidget.cpp \
    volumeview.cpp \
    smartcolorbarwidget.cpp \
    volumeitemsdialog.cpp \
    volumeitem.cpp \
    viewitemmodel.cpp \
    displayoptionsdialog.cpp \
    displayoptions.cpp \
    markeritem.cpp \
    wellitemsdialog.cpp \
    wellitem.cpp \
    markeritemsdialog.cpp \
    volumepicker.cpp \
    playerdialog.cpp \
    horizonitemsdialog.cpp \
    tableitemsdialog.cpp \
    horizonitem.cpp \
    tableitem.cpp \
    viewitem.cpp \
    sliceviewer.cpp

HEADERS += \
    libsliceviewer.h \
    libsliceviewer_global.h \
    compasswidget.h \
    volumeview.h \
    volumeitemsdialog.h \
    volumeitem.h \
    smartcolorbarwidget.h \
    viewitemmodel.h \
    displayoptions.h \
    markeritem.h \
    wellitem.h \
    volumepicker.h \
    markeritemsdialog.h \
    playerdialog.h \
    tableitemsdialog.h \
    horizonitem.h \
    tableitem.h \
    horizonitemsdialog.h \
    wellitemsdialog.h \
    viewitem.h \
    displayoptionsdialog.h \
    statistics.h \
    sliceviewer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    displayoptionsdialog.ui \
    wellitemsdialog.ui \
    markeritemsdialog.ui \
    volumeitemsdialog.ui \
    playerdialog.ui \
    horizonitemsdialog.ui \
    tableitemsdialog.ui \
    sliceviewer.ui

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
