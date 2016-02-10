#-------------------------------------------------
#
# Project created by QtCreator 2015-11-30T12:12:53
#
#-------------------------------------------------

QT       += widgets

QT       -= gui


CONFIG += c++11

win32{
    CONFIG+=staticlib
}

include(../common.pri)


TARGET = utilUI
TEMPLATE = lib

DEFINES += UTILUI_LIBRARY

SOURCES += utilui.cpp \
    rulergraphicsview.cpp \ 
    linerangeselectiondialog.cpp \
    datapointitem.cpp \
    baseviewer.cpp \
    pointdispatcher.cpp \
    twocombosdialog.cpp

HEADERS += utilui.h\
        utilui_global.h \
    rulergraphicsview.h \ 
    linerangeselectiondialog.h \
    datapointitem.h \
    baseviewer.h \
    pointdispatcher.h \
    twocombosdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    linerangeselectiondialog.ui \
    twocombosdialog.ui
