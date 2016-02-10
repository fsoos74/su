#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:21:12
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = SeismicUI
TEMPLATE = lib

include(../common.pri)
win32{
    CONFIG +=staticlib
}

DEFINES += SEISMICUI_LIBRARY

SOURCES += seismicui.cpp \
    segyinputdialog.cpp \
    headerdialog.cpp \
    ebcdicdialog.cpp \
    navigationwidget.cpp \
    gatherlabel.cpp \
    gatherview.cpp \
    gatherruler.cpp \
    selectheaderwordsdialog.cpp \
    gatherviewer.cpp \
    gatherscaler.cpp \
    axxislabelwidget.cpp \
    headerui.cpp \
    volumedisplayoptionsdialog.cpp \
    tracedisplayoptionsdialog.cpp \
    tracescalingdialog.cpp

HEADERS += seismicui.h\
        seismicui_global.h \
    segyinputdialog.h \
    headerdialog.h \
    ebcdicdialog.h \
    navigationwidget.h \
    gatherlabel.h \
    gatherview.h \
    gatherruler.h \
    selectheaderwordsdialog.h \
    gatherviewer.h \
    gatherscaler.h \
    axxislabelwidget.h \
    headerui.h \
    volumedisplayoptionsdialog.h \
    tracedisplayoptionsdialog.h \
    tracescalingdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    segyinputdialog.ui \
    headerdialog.ui \
    ebcdicdialog.ui \
    navigationwidget.ui \
    gatherviewer.ui \
    volumedisplayoptionsdialog.ui \
    tracedisplayoptionsdialog.ui \
    tracescalingdialog.ui



RESOURCES += \
    icons.qrc



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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Seismic/release/ -lSeismic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Seismic/debug/ -lSeismic
else:unix: LIBS += -L$$OUT_PWD/../Seismic/ -lSeismic

INCLUDEPATH += $$PWD/../Seismic
DEPENDPATH += $$PWD/../Seismic


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Database/release/ -lDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Database/debug/ -lDatabase
else:unix: LIBS += -L$$OUT_PWD/../Database/ -lDatabase

INCLUDEPATH += $$PWD/../Database
DEPENDPATH += $$PWD/../Database

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil

INCLUDEPATH += $$PWD/../util
DEPENDPATH += $$PWD/../util

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilUI/release/ -lutilUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilUI/debug/ -lutilUI
else:unix: LIBS += -L$$OUT_PWD/../utilUI/ -lutilUI

INCLUDEPATH += $$PWD/../utilUI
DEPENDPATH += $$PWD/../utilUI
