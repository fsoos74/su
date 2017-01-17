#-------------------------------------------------
#
# Project created by QtCreator 2015-11-19T15:48:39
#
#-------------------------------------------------

#need gui for QTransform
#QT       -= gui

win32{
    CONFIG+=staticlib
}


include(../common.pri)

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
    cropvolumeprocess.cpp

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
    cropvolumeprocess.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../util/release/ -lutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../util/debug/ -lutil
else:unix: LIBS += -L$$OUT_PWD/../util/ -lutil

INCLUDEPATH += $$PWD/../util
DEPENDPATH += $$PWD/../util

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Statistics/release/ -lStatistics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Statistics/debug/ -lStatistics
else:unix: LIBS += -L$$OUT_PWD/../Statistics/ -lStatistics

INCLUDEPATH += $$PWD/../Statistics
DEPENDPATH += $$PWD/../Statistics
