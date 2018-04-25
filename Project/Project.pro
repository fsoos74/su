#-------------------------------------------------
#
# Project created by QtCreator 2015-11-07T17:55:43
#
#-------------------------------------------------

QT       -= gui

include(../common.pri)

win32{
    CONFIG +=staticlib
}

QT += sql
QT += widgets

TARGET = Project
TEMPLATE = lib

DEFINES += PROJECT_LIBRARY

SOURCES += project.cpp \
    avoproject.cpp \
    xprwriter.cpp \
    xprreader.cpp \
    seismicdatasetinfo.cpp \
    xdiwriter.cpp \
    xdireader.cpp \
    seismicdatasetreader.cpp \
    projectgeometry.cpp \
    axxisorientation.cpp \
    seismicdatasetwriter.cpp \
    xppreader.cpp \
    xppwriter.cpp \
    wellinfo.cpp \
    xwiwriter.cpp \
    xwireader.cpp \
    topsdbmanager.cpp \
    table.cpp \
    tablewriter.cpp \
    tablereader.cpp

HEADERS += project.h\
        project_global.h \
    avoproject.h \
    xprwriter.h \
    xprreader.h \
    seismicdatasetinfo.h \
    xdiwriter.h \
    xdireader.h \
    seismicdatasetreader.h \
    projectgeometry.h \
    axxisorientation.h \
    seismicdatasetwriter.h \
    xppreader.h \
    xppwriter.h \
    processparams.h \
    wellinfo.h \
    xwiwriter.h \
    xwireader.h \
    topsdbmanager.h \
    table.h \
    tablewriter.h \
    tablereader.h \
    tablereader.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Database/release/ -lDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Database/debug/ -lDatabase
else:unix: LIBS += -L$$OUT_PWD/../Database/ -lDatabase

INCLUDEPATH += $$PWD/../Database
DEPENDPATH += $$PWD/../Database

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Well/release/ -lWell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Well/debug/ -lWell
else:unix: LIBS += -L$$OUT_PWD/../Well/ -lWell

INCLUDEPATH += $$PWD/../Well
DEPENDPATH += $$PWD/../Well
