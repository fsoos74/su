#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:42:14
#
#-------------------------------------------------

QT       -= gui


TARGET = Grid
TEMPLATE = lib

include(../common.pri)
#CONFIG +=staticlib

DEFINES += GRID_LIBRARY

SOURCES += grid.cpp \
    grid2d.cpp

HEADERS += grid.h\
        grid_global.h \
    grid2d.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil

INCLUDEPATH += $$PWD/../util
DEPENDPATH += $$PWD/../util
