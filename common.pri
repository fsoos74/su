DEFINES += COMPANY=\\\"SeismicUtensils\\\"
DEFINES += PROGRAM=\\\"AVO-Detect\\\"
DEFINES += VERSION=\\\"1.2.3\\\"

CONFIG += c++11

unix{
    QMAKE_CXXFLAGS += -O3
}
