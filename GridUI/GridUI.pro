#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:48:10
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = GridUI
TEMPLATE = lib

DEFINES += GRIDUI_LIBRARY

SOURCES += gridui.cpp

HEADERS += gridui.h\
        gridui_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
