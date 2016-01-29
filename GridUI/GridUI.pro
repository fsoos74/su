#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T15:48:10
#
#-------------------------------------------------

QT       += widgets

//QT       -= gui

TARGET = GridUI
TEMPLATE = lib

include(../common.pri)
#CONFIG +=staticlib


DEFINES += GRIDUI_LIBRARY

SOURCES += gridui.cpp \
    gridview.cpp \
    displayrangedialog.cpp \
    gridimportdialog.cpp \
    isolinedialog.cpp \
    orientationdialog.cpp \
    axxisorientation.cpp \
    gridviewer.cpp \
    griddisplayoptionsdialog.cpp \
    gridexportdialog.cpp

HEADERS += gridui.h\
        gridui_global.h \
    gridview.h \
    displayrangedialog.h \
    gridimportdialog.h \
    isolinedialog.h \
    orientationdialog.h \
    axxisorientation.h \
    gridviewer.h \
    griddisplayoptionsdialog.h \
    gridexportdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}



FORMS += \
    displayrangedialog.ui \
    gridimportdialog.ui \
    isolinedialog.ui \
    orientationdialog.ui \
    gridviewer.ui \
    griddisplayoptionsdialog.ui \
    gridexportdialog.ui



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil

INCLUDEPATH += $$PWD/../util
DEPENDPATH += $$PWD/../util

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/release/ -lColorUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/debug/ -lColorUI
else:unix: LIBS += -L$$OUT_PWD/../ColorUI/ -lColorUI

INCLUDEPATH += $$PWD/../ColorUI
DEPENDPATH += $$PWD/../ColorUI

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Grid/release/ -lGrid
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Grid/debug/ -lGrid
else:unix: LIBS += -L$$OUT_PWD/../Grid/ -lGrid

INCLUDEPATH += $$PWD/../Grid
DEPENDPATH += $$PWD/../Grid


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilUI/release/ -lutilUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilUI/debug/ -lutilUI
else:unix: LIBS += -L$$OUT_PWD/../utilUI/ -lutilUI

INCLUDEPATH += $$PWD/../utilUI
DEPENDPATH += $$PWD/../utilUI
