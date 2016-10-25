#-------------------------------------------------
#
# Project created by QtCreator 2015-11-19T16:13:41
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

include(../common.pri)

TARGET = ProzessUI
TEMPLATE = lib

win32{
    CONFIG += staticlib
}

DEFINES += PROZESSUI_LIBRARY

SOURCES += prozessui.cpp \
    createtimeslicedialog.cpp \
    horizonsemblancedialog.cpp \
    computeinterceptgradientdialog.cpp \
    horizonamplitudesdialog.cpp \
    interceptgradientvolumedialog.cpp \
    fluidfactordialog.cpp \
    fluidfactorvolumedialog.cpp \
    processparametersdialog.cpp \
    gridrunuserscriptdialog.cpp \
    runvolumescriptdialog.cpp \
    exportseismicdialog.cpp \
    exportvolumedialog.cpp \
    amplitudevolumedialog.cpp \
    semblancevolumedialog.cpp \
    secondaryavoattributesdialog.cpp \
    trendbasedattributesdialog.cpp

HEADERS += prozessui.h\
        prozessui_global.h \
    createtimeslicedialog.h \
    horizonsemblancedialog.h \
    computeinterceptgradientdialog.h \
    horizonamplitudesdialog.h \
    interceptgradientvolumedialog.h \
    fluidfactordialog.h \
    fluidfactorvolumedialog.h \
    processparametersdialog.h \
    gridrunuserscriptdialog.h \
    runvolumescriptdialog.h \
    exportseismicdialog.h \
    exportvolumedialog.h \
    amplitudevolumedialog.h \
    semblancevolumedialog.h \
    secondaryavoattributesdialog.h \
    trendbasedattributesdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    createtimeslicedialog.ui \
    horizonsemblancedialog.ui \
    computeinterceptgradientdialog.ui \
    horizonamplitudesdialog.ui \
    interceptgradientvolumedialog.ui \
    fluidfactordialog.ui \
    fluidfactorvolumedialog.ui \
    gridrunuserscriptdialog.ui \
    runvolumescriptdialog.ui \
    exportseismicdialog.ui \
    exportvolumedialog.ui \
    amplitudevolumedialog.ui \
    semblancevolumedialog.ui \
    secondaryavoattributesdialog.ui \
    trendbasedattributesdialog.ui


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Grid/release/ -lGrid
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Grid/debug/ -lGrid
else:unix: LIBS += -L$$OUT_PWD/../Grid/ -lGrid

INCLUDEPATH += $$PWD/../Grid
DEPENDPATH += $$PWD/../Grid

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil

INCLUDEPATH += $$PWD/../util
DEPENDPATH += $$PWD/../util
