#-------------------------------------------------
#
# Project created by QtCreator 2015-11-19T15:48:39
#
#-------------------------------------------------

#need gui for QTransform
#QT       -= gui
QT+=widgets     #qApp
QT+=sql

win32{
    CONFIG+=staticlib
}


include(../common.pri)

#this is in common.prj now
#QMAKE_CXXFLAGS += -fopenmp
#QMAKE_LFLAGS += -fopenmp

TARGET = prozess
TEMPLATE = lib

DEFINES += PROZESS_LIBRARY

SOURCES += prozess.cpp \
    projectprocess.cpp \
    createtimesliceprocess.cpp \
    horizonsemblanceprocess.cpp \
    computeinterceptgradientprocess.cpp \
    horizonamplitudesprocess.cpp \
    windowreductionfunction.cpp \
    utilities.cpp \
    rotatinggatherbuffer.cpp \
    gatherbuffer.cpp \
    interceptgradientvolumeprocess.cpp \
    fluidfactorprocess.cpp \
    fluidfactorvolumeprocess.cpp \
    amplitudecurves.cpp \
    gatherfilter.cpp \
    rungridscriptprocess.cpp \
    runvolumescriptprocess.cpp \
    exportseismicprocess.cpp \
    exportvolumeprocess.cpp \
    amplitudevolumeprocess.cpp \
    semblancevolumeprocess.cpp \
    semblance.cpp \
    secondaryattributesprocess.cpp \
    secondaryattributevolumesprocess.cpp \
    trendbasedattributesprocess.cpp \
    trendbasedattributevolumesprocess.cpp \
    cropvolumeprocess.cpp \
    convertgridprocess.cpp \
    smoothgridprocess.cpp \
    cropgridprocess.cpp \
    curvaturevolumeprocess.cpp \
    horizonfrequenciesprocess.cpp \
    frequencyvolumeprocess.cpp \
    horizoncurvatureprocess.cpp \
    gridmathprocess.cpp \
    hilbert.cpp \
    instantaneousattributesprocess.cpp \
    extracttimesliceprocess.cpp \
    extractdatasetprocess.cpp \
    offsetstackprocess.cpp \
    cropdatasetprocess.cpp \
    replacebadtracesprocess.cpp \
    variancevolumeprocess.cpp \
    flattenvolumeprocess.cpp \
    unflattenvolumeprocess.cpp \
    volumestatisticsprocess.cpp \
    buildwellvolumeprocess.cpp \
    volumemathprocess.cpp \
    frequencyspectra.cpp \
    createvolumeprocess.cpp \
    mathprocessor.cpp \
    creategridprocess.cpp \
    exportgridprocess.cpp \
    gridformat.cpp \
    importgridprocess.cpp \
    logmathprocess.cpp \
    logscriptprocess.cpp \
    smoothprocessor.cpp \
    smoothlogprocess.cpp \
    logintegrationprocess.cpp \
    volumedipprocess.cpp \
    volumediptracker.cpp \
    importmarkersprocess.cpp \
    fmcdp2dprocess.cpp \
    volumedipscanprocess.cpp \
    nn.cpp \
    xnlwriter.cpp \
    xnlreader.cpp \
    nnloginterpolationprocess.cpp \
    nnlogprocess.cpp \
    nnlogtrainer.cpp \
    vshaleprocess.cpp \
    exportlogprocess.cpp \
    punchoutvolumeprocess.cpp \
    horizontotopprocess.cpp \
    tabletohorizonprocess.cpp \
    exporttableprocess.cpp \
    stacktogatherprocess.cpp

HEADERS += prozess.h\
        prozess_global.h \
    projectprocess.h \
    createtimesliceprocess.h \
    horizonsemblanceprocess.h \
    computeinterceptgradientprocess.h \
    horizonamplitudesprocess.h \
    windowreductionfunction.h \
    utilities.h \
    rotatinggatherbuffer.h \
    gatherbuffer.h \
    interceptgradientvolumeprocess.h \
    fluidfactorprocess.h \
    fluidfactorvolumeprocess.h \
    amplitudecurves.h \
    gatherfilter.h \
    rungridscriptprocess.h \
    runvolumescriptprocess.h \
    exportseismicprocess.h \
    exportvolumeprocess.h \
    amplitudevolumeprocess.h \
    semblancevolumeprocess.h \
    semblance.h \
    secondaryattributesprocess.h \
    secondaryattributevolumesprocess.h \
    trendbasedattributesprocess.h \
    trendbasedattributevolumesprocess.h \
    cropvolumeprocess.h \
    convertgridprocess.h \
    smoothgridprocess.h \
    cropgridprocess.h \
    curvaturevolumeprocess.h \
    horizonfrequenciesprocess.h \
    frequencyvolumeprocess.h \
    horizoncurvatureprocess.h \
    gridmathprocess.h \
    curvature_attributes.h \
    hilbert.h \
    instantaneousattributesprocess.h \
    extracttimesliceprocess.h \
    processparams.h \
    extractdatasetprocess.h \
    offsetstackprocess.h \
    cropdatasetprocess.h \
    replacebadtracesprocess.h \
    variancevolumeprocess.h \
    flattenvolumeprocess.h \
    unflattenvolumeprocess.h \
    volumestatisticsprocess.h \
    buildwellvolumeprocess.h \
    volumemathprocess.h \
    frequencyspectra.h \
    createvolumeprocess.h \
    mathprocessor.h \
    creategridprocess.h \
    exportgridprocess.h \
    gridformat.h \
    importgridprocess.h \
    logmathprocess.h \
    logscriptprocess.h \
    smoothprocessor.h \
    smoothlogprocess.h \
    logintegrationprocess.h \
    volumedipprocess.h \
    volumediptracker.h \
    importmarkersprocess.h \
    fmcdp2dprocess.h \
    volumedipscanprocess.h \
    nn.h \
    matrix.h \
    nnfunc.h \
    xnlwriter.h \
    xnlreader.h \
    nnloginterpolationprocess.h \
    nnlogprocess.h \
    nnlogtrainer.h \
    vshaleprocess.h \
    exportlogprocess.h \
    punchoutvolumeprocess.h \
    horizontotopprocess.h \
    tabletohorizonprocess.h \
    exporttableprocess.h \
    stacktogatherprocess.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#embed python
unix {
    INCLUDEPATH +=/usr/include/python2.7
    #LIBS += -lpython2.7 -lpthread -ldl -lutil -lm -Xlinker -export-dynamic
    #QMAKE_CFLAGS += `/usr/bin/python2.7-config --cflags`;
}
win32{
    INCLUDEPATH += C:\Python27\include
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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Database/release/ -lDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Database/debug/ -lDatabase
else:unix: LIBS += -L$$OUT_PWD/../Database/ -lDatabase

INCLUDEPATH += $$PWD/../Database
DEPENDPATH += $$PWD/../Database

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Seismic/release/ -lSeismic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Seismic/debug/ -lSeismic
else:unix: LIBS += -L$$OUT_PWD/../Seismic/ -lSeismic

INCLUDEPATH += $$PWD/../Seismic
DEPENDPATH += $$PWD/../Seismic

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Well/release/ -lWell
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Well/debug/ -lWell
else:unix: LIBS += -L$$OUT_PWD/../Well/ -lWell

INCLUDEPATH += $$PWD/../Well
DEPENDPATH += $$PWD/../Well

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Statistics/release/ -lStatistics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Statistics/debug/ -lStatistics
else:unix: LIBS += -L$$OUT_PWD/../Statistics/ -lStatistics

INCLUDEPATH += $$PWD/../Statistics
DEPENDPATH += $$PWD/../Statistics

FORMS += \
    nnlogtrainer.ui
