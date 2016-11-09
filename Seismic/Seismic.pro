#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:03:26
#
#-------------------------------------------------

QT       -= gui

TARGET = Seismic
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG +=staticlib
}


DEFINES += SEISMIC_LIBRARY

SOURCES += seismic.cpp \
    segy_text_header.cpp \
    segyreader.cpp \
    segyinfo.cpp \
    xsiwriter.cpp \
    segysampleformat.cpp \
    segy_header_def.cpp \
    xsireader.cpp \
    segywriter.cpp \
    seis_bits.cpp \
    swsegywriter.cpp \
    segyformaterror.cpp

HEADERS += seismic.h\
        seismic_global.h \
    trace.h \
    seis_bits.h \
    segy_traceheader_def.h \
    segy_text_header.h \
    segyreader.h \
    segy_header_def.h \
    segy_data.h \
    segy_binary_header_def.h \
    headervalue.h \
    header.h \
    gather.h \
    ebcdic.h \
    segyinfo.h \
    segysampleformat.h \
    xsiwriter.h \
    xsireader.h \
    segywriter.h \
    seisware_header_def.h \
    swsegywriter.h \
    segyformaterror.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
