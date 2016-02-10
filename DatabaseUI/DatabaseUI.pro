#-------------------------------------------------
#
# Project created by QtCreator 2015-11-09T15:06:37
#
#-------------------------------------------------

TARGET = DatabaseUI
TEMPLATE = lib

include(../common.pri)
QT  += sql
QT       += widgets

win32{
    CONFIG += staticlib
}

DEFINES += DATABASEUI_LIBRARY

SOURCES += databaseui.cpp \
    cdpdatabaseviewer.cpp \
    databaseviewer.cpp

HEADERS += databaseui.h\
        databaseui_global.h \
    cdpdatabaseviewer.h \
    databaseviewer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    cdpdatabaseviewer.ui \
    databaseviewer.ui


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Database/release/ -lDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Database/debug/ -lDatabase
else:unix: LIBS += -L$$OUT_PWD/../Database/ -lDatabase

INCLUDEPATH += $$PWD/../Database
DEPENDPATH += $$PWD/../Database
