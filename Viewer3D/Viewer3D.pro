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
    slicedef.cpp \
    vic.cpp \
    vit.cpp \
    renderitem.cpp \
    renderscene.cpp \
    navigation3dcontrols.cpp \
    volumenavigationdialog.cpp \
    editslicesdialog.cpp \
    edithorizonsdialog.cpp \
    slicemodel.cpp \
    horizonmodel.cpp \
    overlaypercentagedialog.cpp \
    horizondef.cpp \
    volumedef.cpp \
    volumemodel.cpp

HEADERS  += \
    viewwidget.h \
    renderengine.h \
    viewer3d.h \
    viewer3d_global.h \
    volumeviewer.h \
    modaldisplayrangedialog.h \
    slicedef.h \
    vertexdata.h \
    vic.h \
    vit.h \
    renderitem.h \
    renderscene.h \
    navigation3dcontrols.h \
    volumenavigationdialog.h \
    editslicesdialog.h \
    edithorizonsdialog.h \
    sliceparameters.h \
    slicemodel.h \
    horizondef.h \
    horizonmodel.h \
    overlaypercentagedialog.h \
    volumedef.h \
    volumemodel.h

FORMS    += \
    volumeviewer.ui \
    modaldisplayrangedialog.ui \
    navigation3dcontrols.ui \
    volumenavigationdialog.ui \
    editslicesdialog.ui \
    edithorizonsdialog.ui \
    overlaypercentagedialog.ui

RESOURCES += \
    shaders.qrc

DISTFILES += \
    fshader.glsl \
    vshader.glsl

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

#need this for displayrangedialog
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GridUI/release/ -lGridUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GridUI/debug/ -lGridUI
else:unix: LIBS += -L$$OUT_PWD/../GridUI/ -lGridUI

INCLUDEPATH += $$PWD/../GridUI
DEPENDPATH += $$PWD/../GridUI

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Statistics/release/ -lStatistics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Statistics/debug/ -lStatistics
else:unix: LIBS += -L$$OUT_PWD/../Statistics/ -lStatistics

INCLUDEPATH += $$PWD/../Statistics
DEPENDPATH += $$PWD/../Statistics


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StatisticsUI/release/ -lStatisticsUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StatisticsUI/debug/ -lStatisticsUI
else:unix: LIBS += -L$$OUT_PWD/../StatisticsUI/ -lStatisticsUI

INCLUDEPATH += $$PWD/../StatisticsUI
DEPENDPATH += $$PWD/../StatisticsUI

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Well/release/ -lWell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Well/debug/ -lWell
else:unix: LIBS += -L$$OUT_PWD/../Well/ -lWell

INCLUDEPATH += $$PWD/../Well
DEPENDPATH += $$PWD/../Well
