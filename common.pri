DEFINES += COMPANY=\\\"SeismicUtensils\\\"
DEFINES += PROGRAM=\\\"AVO-Detect\\\"
DEFINES += VERSION=\\\"1.9.31\\\"

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


# static build on windows
# need this define because q_decl_export creates linkage errors when used with static libraries!!!! NEVER forget this!!!
win32{
    #CONFIG+=staticlib      # NEVER DO THIS FOR EXECUTABLE!!!!
    DEFINES+=STATICBUILD
}
