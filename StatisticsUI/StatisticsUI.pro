#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T14:02:43
#
#-------------------------------------------------

QT       += widgets

TARGET = StatisticsUI
TEMPLATE = lib

include(../common.pri)

win32{
    CONFIG +=staticlib
}

DEFINES += STATISTICSUI_LIBRARY

SOURCES += statisticsui.cpp\
            histogramdialog.cpp \
    histogramrangeselectiondialog.cpp \
    histogramwidget.cpp \
    histogramcolorbarwidget.cpp \
    histogramcreator.cpp

HEADERS += statisticsui.h\
        statisticsui_global.h\
        histogramdialog.h \
    histogramrangeselectiondialog.h \
    histogramwidget.h \
    histogramcolorbarwidget.h \
    histogramcreator.h

FORMS += histogramdialog.ui \
    histogramrangeselectiondialog.ui

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Statistics/release/ -lStatistics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Statistics/debug/ -lStatistics
else:unix: LIBS += -L$$OUT_PWD/../Statistics/ -lStatistics

INCLUDEPATH += $$PWD/../Statistics
DEPENDPATH += $$PWD/../Statistics


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utilUI/release/ -lutilUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utilUI/debug/ -lutilUI
else:unix: LIBS += -L$$OUT_PWD/../utilUI/ -lutilUI

INCLUDEPATH += $$PWD/../utilUI
DEPENDPATH += $$PWD/../utilUI


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/release/ -lColorUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ColorUI/debug/ -lColorUI
else:unix: LIBS += -L$$OUT_PWD/../ColorUI/ -lColorUI

INCLUDEPATH += $$PWD/../ColorUI
DEPENDPATH += $$PWD/../ColorUI

