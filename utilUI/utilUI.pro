#-------------------------------------------------
#
# Project created by QtCreator 2015-11-30T12:12:53
#
#-------------------------------------------------

QT       += widgets

QT       -= gui


CONFIG += c++11

win32{
    CONFIG+=staticlib
}

include(../common.pri)


TARGET = utilUI
TEMPLATE = lib

DEFINES += UTILUI_LIBRARY

SOURCES += utilui.cpp \
    rulergraphicsview.cpp \ 
    linerangeselectiondialog.cpp \
    datapointitem.cpp \
    baseviewer.cpp \
    pointdispatcher.cpp \
    twocombosdialog.cpp \
    alignedtextgraphicsitem.cpp \
    activetextgraphicsitem.cpp \
    selectionpoint.cpp \
    pointdisplayoptionsdialog.cpp \
    selectionpointbuffer.cpp \
    mousemodeselector.cpp \
    mousemode.cpp \
    reversespinbox.cpp

HEADERS += utilui.h\
        utilui_global.h \
    rulergraphicsview.h \ 
    linerangeselectiondialog.h \
    datapointitem.h \
    baseviewer.h \
    pointdispatcher.h \
    twocombosdialog.h \
    alignedtextgraphicsitem.h \
    activetextgraphicsitem.h \
    selectionpoint.h \
    pointdisplayoptionsdialog.h \
    selectionpointbuffer.h \
    mousemodeselector.h \
    mousemode.h \
    reversespinbox.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    linerangeselectiondialog.ui \
    twocombosdialog.ui \
    pointdisplayoptionsdialog.ui \
    mousemodeselector.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/release/ -lColorUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/debug/ -lColorUI
else:unix: LIBS += -L$$OUT_PWD/../ColorUI/ -lColorUI

INCLUDEPATH += $$PWD/../ColorUI
DEPENDPATH += $$PWD/../ColorUI

RESOURCES += \
    ../icons.qrc

DISTFILES += \
    ../images/arrow-32x32.png \
    ../images/select-32x32.png \
    ../images/Zoom.png
