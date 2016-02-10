#-------------------------------------------------
#
# Project created by QtCreator 2015-10-04T16:49:11
#
#-------------------------------------------------

include(../common.pri)

QT       -= gui

win32{
    CONFIG += staticlib
}

TARGET = util
TEMPLATE = lib

DEFINES += UTIL_LIBRARY

SOURCES += util.cpp

HEADERS += util.h\
        util_global.h \
    array2d.h \
    rectangle.h \
    array3d.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
