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
    trendbasedattributesdialog.cpp \
    cropvolumedialog.cpp \
    convertgriddialog.cpp \
    smoothgriddialog.cpp \
    cropgriddialog.cpp \
    curvaturevolumedialog.cpp \
    horizonfrequenciesdialog.cpp \
    frequencyvolumedialog.cpp \
    gridmathdialog.cpp \
    curvaturedialog.cpp \
    instantaneousattributesdialog.cpp \
    extractdatasetdialog.cpp \
    offsetstackdialog.cpp \
    cropdatasetdialog.cpp \
    replacebadtracesdialog.cpp \
    flattenvolumedialog.cpp \
    unflattenvolumedialog.cpp \
    volumestatisticsdialog.cpp \
    buildwellvolumedialog.cpp \
    volumemathdialog.cpp \
    createvolumedialog.cpp \
    creategriddialog.cpp \
    exportgriddialog.cpp \
    importgriddialog.cpp \
    logmathdialog.cpp \
    logscriptdialog.cpp \
    smoothlogdialog.cpp \
    logintegrationdialog.cpp \
    volumedipdialog.cpp \
    importmarkersdialog.cpp \
    fmcdp2ddialog.cpp \
    dipscandialog.cpp \
    nnloginterpolationdialog.cpp \
    nnlogdialog.cpp \
    vshaledialog.cpp \
    exportlogdialog.cpp \
    punchoutvolumedialog.cpp \
    horizontotopdialog.cpp \
    tabletohorizondialog.cpp \
    exporttabledialog.cpp \
    stacktogatherdialog.cpp \
    volumeiotestdialog.cpp

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
    trendbasedattributesdialog.h \
    cropvolumedialog.h \
    convertgriddialog.h \
    smoothgriddialog.h \
    cropgriddialog.h \
    curvaturevolumedialog.h \
    horizonfrequenciesdialog.h \
    frequencyvolumedialog.h \
    gridmathdialog.h \
    curvaturedialog.h \
    instantaneousattributesdialog.h \
    extractdatasetdialog.h \
    offsetstackdialog.h \
    cropdatasetdialog.h \
    replacebadtracesdialog.h \
    flattenvolumedialog.h \
    unflattenvolumedialog.h \
    volumestatisticsdialog.h \
    buildwellvolumedialog.h \
    volumemathdialog.h \
    createvolumedialog.h \
    creategriddialog.h \
    exportgriddialog.h \
    importgriddialog.h \
    logmathdialog.h \
    logscriptdialog.h \
    smoothlogdialog.h \
    logintegrationdialog.h \
    volumedipdialog.h \
    importmarkersdialog.h \
    fmcdp2ddialog.h \
    dipscandialog.h \
    nnloginterpolationdialog.h \
    nnlogdialog.h \
    vshaledialog.h \
    exportlogdialog.h \
    punchoutvolumedialog.h \
    horizontotopdialog.h \
    tabletohorizondialog.h \
    exporttabledialog.h \
    stacktogatherdialog.h \
    volumeiotestdialog.h

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
    trendbasedattributesdialog.ui \
    cropvolumedialog.ui \
    convertgriddialog.ui \
    smoothgriddialog.ui \
    cropgriddialog.ui \
    curvaturevolumedialog.ui \
    horizonfrequenciesdialog.ui \
    frequencyvolumedialog.ui \
    gridmathdialog.ui \
    curvaturedialog.ui \
    instantaneousattributesdialog.ui \
    extractdatasetdialog.ui \
    offsetstackdialog.ui \
    cropdatasetdialog.ui \
    replacebadtracesdialog.ui \
    flattenvolumedialog.ui \
    unflattenvolumedialog.ui \
    volumestatisticsdialog.ui \
    buildwellvolumedialog.ui \
    volumemathdialog.ui \
    createvolumedialog.ui \
    creategriddialog.ui \
    exportgriddialog.ui \
    importgriddialog.ui \
    logmathdialog.ui \
    logscriptdialog.ui \
    smoothlogdialog.ui \
    logintegrationdialog.ui \
    volumedipdialog.ui \
    importmarkersdialog.ui \
    fmcdp2ddialog.ui \
    dipscandialog.ui \
    nnloginterpolationdialog.ui \
    nnlogdialog.ui \
    vshaledialog.ui \
    exportlogdialog.ui \
    punchoutvolumedialog.ui \
    horizontotopdialog.ui \
    tabletohorizondialog.ui \
    exporttabledialog.ui \
    stacktogatherdialog.ui \
    volumeiotestdialog.ui


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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Well/release/ -lWell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Well/debug/ -lWell
else:unix: LIBS += -L$$OUT_PWD/../Well/ -lWell

INCLUDEPATH += $$PWD/../Well
DEPENDPATH += $$PWD/../Well


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../prozess/release/ -lprozess
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../prozess/debug/ -lprozess
else:unix: LIBS += -L$$OUT_PWD/../prozess/ -lprozess

INCLUDEPATH += $$PWD/../prozess
DEPENDPATH += $$PWD/../prozess

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
