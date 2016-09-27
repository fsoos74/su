#-------------------------------------------------
#
# Project created by QtCreator 2015-10-12T14:43:21
#
#-------------------------------------------------

QT       += widgets

#QT       -= gui

include(../common.pri)


win32{
    CONFIG += staticlib
}

TARGET = ColorUI
TEMPLATE = lib

DEFINES += COLORUI_LIBRARY

SOURCES += colorui.cpp \
    colortable.cpp \
    colorbarwidget.cpp \
    colortabledialog.cpp \
    xctwriter.cpp \
    xctreader.cpp \
    colorbutton.cpp \
    colorbarconfigurationdialog.cpp

HEADERS += colorui.h\
        colorui_global.h \
    colortable.h \
    colorbarwidget.h \
    colortabledialog.h \
    xctreader.h \
    xctwriter.h \
    colorbutton.h \
    colorbarconfigurationdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    colorbarconfigurationdialog.ui
