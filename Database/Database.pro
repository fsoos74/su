#-------------------------------------------------
#
# Project created by QtCreator 2015-11-09T12:17:51
#
#-------------------------------------------------

QT       -= gui
QT  += sql

include(../common.pri)

win32{
    CONFIG += staticlib
}

TARGET = Database
TEMPLATE = lib

DEFINES += DATABASE_LIBRARY

SOURCES += database.cpp \
    cdpdatabase.cpp

HEADERS += database.h\
        database_global.h \
    cdpdatabase.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
