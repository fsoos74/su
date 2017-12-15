#-------------------------------------------------
#
# Project created by QtCreator 2017-08-21T10:04:43
#
#-------------------------------------------------

QT       -= gui
QT      += widgets  # qApp

TARGET = Well
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG +=staticlib
}


DEFINES += WELL_LIBRARY

SOURCES += well.cpp \
    las_data.cpp \
    las_mnemonic.cpp \
    las_reader.cpp \
    log.cpp \
    logwriter.cpp \
    logreader.cpp \
    wellpathwriter.cpp \
    wellpathreader.cpp \
    stringio.cpp \
    wellpath.cpp \
    wellmarker.cpp

HEADERS += well.h\
        well_global.h \
    las_data.h \
    las_mnemonic.h \
    las_reader.h \
    log.h \
    logwriter.h \
    logreader.h \
    wellpath.h \
    wellpathwriter.h \
    wellpathreader.h \
    stringio.h \
    wellmarker.h \
    wellmarkers.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
#else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil
#INCLUDEPATH += $$PWD/../util
#DEPENDPATH += $$PWD/../util

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilfs/release/ -lutilfs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilfs/debug/ -lutilfs
else:unix: LIBS += -L$$OUT_PWD/../utilfs/ -lutilfs

INCLUDEPATH += $$PWD/../utilfs
DEPENDPATH += $$PWD/../utilfs
