#-------------------------------------------------
#
# Project created by QtCreator 2016-03-01T16:06:03
#
#-------------------------------------------------

include(../common.pri)

QT       -= gui

TARGET = Statistics
TEMPLATE = lib

DEFINES += STATISTICS_LIBRARY

win32{
    CONFIG +=staticlib
}

SOURCES += statistics.cpp \
    histogram.cpp \
    linearregression.cpp

HEADERS += statistics.h\
        statistics_global.h \
    histogram.h \
    linearregression.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
