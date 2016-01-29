#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:42:14
#
#-------------------------------------------------

QT       -= gui


TARGET = Grid
TEMPLATE = lib

include(../common.pri)

#CONFIG += staticlib

DEFINES += GRID_LIBRARY

SOURCES += grid.cpp \
    grid2d.cpp \
    xgrwriter.cpp \
    xgrreader.cpp \
    isolinecomputer.cpp \
    grid3d.cpp \
    volumewriter.cpp \
    volumereader.cpp

HEADERS += grid.h\
        grid_global.h \
    grid2d.h \
    xgrwriter.h \
    xgrreader.h \
    isolinecomputer.h \
    grid3d.h \
    volumewriter.h \
    volumereader.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil

INCLUDEPATH += $$PWD/../util
DEPENDPATH += $$PWD/../util

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil

INCLUDEPATH += $$PWD/../util
DEPENDPATH += $$PWD/../util
