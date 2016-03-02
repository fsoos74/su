#-------------------------------------------------
#
# Project created by QtCreator 2015-11-19T15:48:39
#
#-------------------------------------------------

QT       -= gui

win32{
    CONFIG+=staticlib
}

include(../common.pri)

TARGET = prozess
TEMPLATE = lib

DEFINES += PROZESS_LIBRARY

SOURCES += prozess.cpp \
    projectprocess.cpp \
    createtimesliceprocess.cpp \
    horizonsemblanceprocess.cpp \
    computeinterceptgradientprocess.cpp \
    horizonamplitudesprocess.cpp \
    windowreductionfunction.cpp \
    utilities.cpp \
    rotatinggatherbuffer.cpp \
    gatherbuffer.cpp \
    interceptgradientvolumeprocess.cpp \
    fluidfactorprocess.cpp \
    fluidfactorvolumeprocess.cpp \
    amplitudecurves.cpp \
    gatherfilter.cpp

HEADERS += prozess.h\
        prozess_global.h \
    projectprocess.h \
    createtimesliceprocess.h \
    horizonsemblanceprocess.h \
    computeinterceptgradientprocess.h \
    horizonamplitudesprocess.h \
    windowreductionfunction.h \
    utilities.h \
    rotatinggatherbuffer.h \
    gatherbuffer.h \
    interceptgradientvolumeprocess.h \
    fluidfactorprocess.h \
    fluidfactorvolumeprocess.h \
    amplitudecurves.h \
    gatherfilter.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Grid/release/ -lGrid
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Grid/debug/ -lGrid
else:unix: LIBS += -L$$OUT_PWD/../Grid/ -lGrid

INCLUDEPATH += $$PWD/../Grid
DEPENDPATH += $$PWD/../Grid

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Database/release/ -lDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Database/debug/ -lDatabase
else:unix: LIBS += -L$$OUT_PWD/../Database/ -lDatabase

INCLUDEPATH += $$PWD/../Database
DEPENDPATH += $$PWD/../Database

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Seismic/release/ -lSeismic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Seismic/debug/ -lSeismic
else:unix: LIBS += -L$$OUT_PWD/../Seismic/ -lSeismic

INCLUDEPATH += $$PWD/../Seismic
DEPENDPATH += $$PWD/../Seismic

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil

INCLUDEPATH += $$PWD/../util
DEPENDPATH += $$PWD/../util

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Statistics/release/ -lStatistics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Statistics/debug/ -lStatistics
else:unix: LIBS += -L$$OUT_PWD/../Statistics/ -lStatistics

INCLUDEPATH += $$PWD/../Statistics
DEPENDPATH += $$PWD/../Statistics
