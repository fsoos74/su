#-------------------------------------------------
#
# Project created by QtCreator 2015-10-04T16:49:11
#
#-------------------------------------------------

QT       -= gui

TARGET = util
TEMPLATE = lib

DEFINES += UTIL_LIBRARY

SOURCES += util.cpp

HEADERS += util.h\
        util_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
