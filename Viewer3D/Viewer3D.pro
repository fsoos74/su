#-------------------------------------------------
#
# Project created by QtCreator 2016-12-07T08:21:43
#
#-------------------------------------------------

QT       += core gui widgets


TARGET = Viewer3D
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG +=staticlib
}

DEFINES += VIEWER3D_LIBRARY

SOURCES += \
    viewwidget.cpp \
    renderengine.cpp \
    viewer3d.cpp \
    volumeviewer.cpp \
    modaldisplayrangedialog.cpp \
    addslicedialog.cpp \
    slicedef.cpp \
    glsceneitem.cpp \
    glscene.cpp \
    glsliceitem.cpp \
    volumenavigationwidget.cpp \
    vic.cpp \
    vit.cpp

HEADERS  += \
    viewwidget.h \
    renderengine.h \
    viewer3d.h \
    viewer3d_global.h \
    volumeviewer.h \
    modaldisplayrangedialog.h \
    addslicedialog.h \
    slicedef.h \
    vertexdata.h \
    glsceneitem.h \
    glscene.h \
    glsliceitem.h \
    volumenavigationwidget.h \
    vic.h \
    vit.h

FORMS    += \
    volumeviewer.ui \
    modaldisplayrangedialog.ui \
    addslicedialog.ui \
    volumenavigationwidget.ui

RESOURCES += \
    shaders.qrc

DISTFILES += \
    fshader.glsl \
    vshader.glsl

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
