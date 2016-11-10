#-------------------------------------------------
#
# Project created by QtCreator 2016-11-10T08:58:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SEGYCrypt
TEMPLATE = app

CONFIG += qtc_runnable
include(../common.pri)

SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Seismic/release/ -lSeismic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Seismic/debug/ -lSeismic
else:unix: LIBS += -L$$OUT_PWD/../Seismic/ -lSeismic

INCLUDEPATH += $$PWD/../Seismic
DEPENDPATH += $$PWD/../Seismic


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Crypt/release/ -lCrypt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Crypt/debug/ -lCrypt
else:unix: LIBS += -L$$OUT_PWD/../Crypt/ -lCrypt

INCLUDEPATH += $$PWD/../Crypt
DEPENDPATH += $$PWD/../Crypt
