#-------------------------------------------------
#
# Project created by QtCreator 2016-11-08T19:57:46
#
#-------------------------------------------------

QT       -= gui

TARGET = Crypt
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG +=staticlib
}


DEFINES += CRYPT_LIBRARY

SOURCES += crypt.cpp \
    cryptfunc.cpp

HEADERS += crypt.h\
        crypt_global.h \
    cryptfunc.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
