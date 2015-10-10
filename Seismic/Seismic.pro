#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:03:26
#
#-------------------------------------------------

QT       -= gui

TARGET = Seismic
TEMPLATE = lib

DEFINES += SEISMIC_LIBRARY

SOURCES += seismic.cpp

HEADERS += seismic.h\
        seismic_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
