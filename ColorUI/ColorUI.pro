#-------------------------------------------------
#
# Project created by QtCreator 2015-10-12T14:43:21
#
#-------------------------------------------------

QT       += widgets

#QT       -= gui

include(../common.pri)


win32{
    CONFIG += staticlib
}

TARGET = ColorUI
TEMPLATE = lib

DEFINES += COLORUI_LIBRARY

SOURCES += colorui.cpp \
    colortable.cpp \
    colorbarwidget.cpp \
    colortabledialog.cpp \
    xctwriter.cpp \
    xctreader.cpp \
    colorbutton.cpp \
    colorbarconfigurationdialog.cpp \
    colortableimportdialog.cpp

HEADERS += colorui.h\
        colorui_global.h \
    colortable.h \
    colorbarwidget.h \
    colortabledialog.h \
    xctreader.h \
    xctwriter.h \
    colorbutton.h \
    colorbarconfigurationdialog.h \
    colortableimportdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    colorbarconfigurationdialog.ui \
    colortableimportdialog.ui

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
#else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil
#INCLUDEPATH += $$PWD/../util
#DEPENDPATH += $$PWD/../util


#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilfs/release/ -lutilfs
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilfs/debug/ -lutilfs
#else:unix: LIBS += -L$$OUT_PWD/../utilfs/ -lutilfs

#INCLUDEPATH += $$PWD/../utilfs
#DEPENDPATH += $$PWD/../utilfs
