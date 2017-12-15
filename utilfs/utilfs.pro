#-------------------------------------------------
#
# Project created by QtCreator 2015-10-04T16:49:11
#
#-------------------------------------------------
#renamed this to utilfs to avoid conflict with system libutil

include(../common.pri)

QT       -= gui

win32{
    CONFIG += staticlib
}

TARGET = utilfs
TEMPLATE = lib

DEFINES += UTILFS_LIBRARY

SOURCES += utilfs.cpp \
    interp.cpp \
    fft.cpp \
    spectrum.cpp  

HEADERS += utilfs.h\
        utilfs_global.h \
    fft.h \
    spectrum.h \
    array2d.h \
    rectangle.h \
    array3d.h \
    range.h \
    interp.h \
    volumedimensions.h \
    tree2d.h   

unix {
    target.path = /usr/lib
    INSTALLS += target
}
