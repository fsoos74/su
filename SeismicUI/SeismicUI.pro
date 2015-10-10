#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:21:12
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = SeismicUI
TEMPLATE = lib

DEFINES += SEISMICUI_LIBRARY

SOURCES += seismicui.cpp

HEADERS += seismicui.h\
        seismicui_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
