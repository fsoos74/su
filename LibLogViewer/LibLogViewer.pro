#-------------------------------------------------
#
# Project created by QtCreator 2019-06-08T14:12:20
#
#-------------------------------------------------

QT       += widgets sql
QT       -= gui

TARGET = LibLogViewer
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG +=staticlib
}

DEFINES += LIBLOGVIEWER_LIBRARY

SOURCES += \
        liblogviewer.cpp \
    multilogviewer.cpp

HEADERS += \
        liblogviewer.h \
        liblogviewer_global.h \  
    multilogviewer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    multilogviewer.ui
