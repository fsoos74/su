DEFINES += COMPANY=\\\"SeismicUtensils\\\"
DEFINES += PROGRAM=\\\"AVO-Detect\\\"
DEFINES += VERSION=\\\"1.4.8\\\"

CONFIG += c++11

win32{
    QMAKE_CFLAGS_RELEASE += /O2 /GL
    QMAKE_LFLAGS_RELEASE += /Gy /OPT:REF,ICF
}


unix{
    QMAKE_CXXFLAGS += -O3
}
