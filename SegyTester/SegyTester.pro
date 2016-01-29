#-------------------------------------------------
#
# Project created by QtCreator 2016-01-08T11:48:36
#
#-------------------------------------------------

QT       += core

QT       -= gui


include(../common.pri)

TARGET = SegyTester
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += qtc_runnable

TEMPLATE = app


SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Seismic/release/ -lSeismic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Seismic/debug/ -lSeismic
else:unix: LIBS += -L$$OUT_PWD/../Seismic/ -lSeismic

INCLUDEPATH += $$PWD/../Seismic
DEPENDPATH += $$PWD/../Seismic
