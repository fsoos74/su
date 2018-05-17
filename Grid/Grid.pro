#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:42:14
#
#-------------------------------------------------

QT       -= gui
QT       += widgets # for qApp processevents

TARGET = Grid
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG += staticlib
}

DEFINES += GRID_LIBRARY

SOURCES += grid.cpp \
    grid2d.cpp \
    xgrwriter.cpp \
    xgrreader.cpp \
    isolinecomputer.cpp \
    grid3d.cpp \
    volumewriter.cpp \
    volumereader.cpp \
    volume.cpp \
    domain.cpp \
    gridtype.cpp \
    volumetype.cpp \
    volumereader2.cpp \
    volumewriter2.cpp

HEADERS += grid.h\
        grid_global.h \
    grid2d.h \
    xgrwriter.h \
    xgrreader.h \
    isolinecomputer.h \
    grid3d.h \
    volumewriter.h \
    volumereader.h \
    volume.h \
    domain.h \
    gridtype.h \
    volumetype.h \
    volumereader2.h \
    volumewriter2.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilfs/release/ -lutilfs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilfs/debug/ -lutilfs
else:unix: LIBS += -L$$OUT_PWD/../utilfs/ -lutilfs

INCLUDEPATH += $$PWD/../utilfs
DEPENDPATH += $$PWD/../utilfs

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
#else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil
#INCLUDEPATH += $$PWD/../util
#DEPENDPATH += $$PWD/../util
