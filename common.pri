DEFINES += COMPANY=\\\"SeismicUtensils\\\"
DEFINES += PROGRAM=\\\"AVO-Detect\\\"
DEFINES += VERSION=\\\"1.7.6\\\"

CONFIG += c++11

#  optimization on windows
win32{
    QMAKE_CFLAGS_RELEASE += /O2 /GL
    QMAKE_LFLAGS_RELEASE += /Gy /OPT:REF,ICF
}

# optimization on linux
unix{
    QMAKE_CXXFLAGS += -O3
}


#openmp parallelization
win32{
    QMAKE_CXXFLAGS += /openmp
    QMAKE_LFLAGS += /openmp
}

unix{
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}
