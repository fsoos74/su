#-------------------------------------------------
#
# Project created by QtCreator 2016-03-01T16:06:03
#
#-------------------------------------------------

QT       -= gui

TARGET = Statistics
TEMPLATE = lib

DEFINES += STATISTICS_LIBRARY

SOURCES += statistics.cpp \
    histogram.cpp

HEADERS += statistics.h\
        statistics_global.h \
    histogram.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
