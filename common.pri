DEFINES += COMPANY=\\\"SeismicUtensils\\\"
DEFINES += PROGRAM=\\\"AVO-Detect\\\"
DEFINES += VERSION=\\\"0.9.3\\\"

CONFIG += c++11

unix{
    QMAKE_CXXFLAGS += -O3
}
