#-------------------------------------------------
#
# Project created by QtCreator 2016-11-08T19:57:46
#
#-------------------------------------------------

QT       -= gui

TARGET = Crypt
TEMPLATE = lib

DEFINES += CRYPT_LIBRARY

SOURCES += crypt.cpp

HEADERS += crypt.h\
        crypt_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
