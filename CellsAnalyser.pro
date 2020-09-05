#-------------------------------------------------
#
# Project created by QtCreator 2015-10-14T15:52:09
#
#-------------------------------------------------

QT       += core gui opengl
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CellsAnalyser
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/apropos.cpp \
    src/app.cpp \
    src/parameters.cpp \
    src/viewercvgl.cpp \
    lib/qcustomplot.cpp \
    src/slicetool.cpp \
    src/player.cpp \
    lib/edimageprocessor.cpp \
    src/component.cpp \
    src/population.cpp \
    src/contours.cpp \
    lib/qmathstools.cpp

HEADERS  += src/include/mainwindow.h \
    src/include/apropos.h \
    src/include/app.h \
    src/include/parameters.h \
    src/include/viewercvgl.h \
    lib/qcustomplot.h \
    src/include/slicetool.h \
    src/include/player.h \
    lib/edimageprocessor.h \
    src/include/component.h \
    src/include/population.h \
    src/include/contours.h \
    lib/qmathstools.h

FORMS    += ui/mainwindow.ui \
    ui/apropos.ui \
    ui/parameters.ui

OPENCV_PATH = /usr/share/opencv

INCLUDEPATH  += /usr/include/opencv2

LIBS     += -L$$OPENCV_PATH -lopencv_core -lopencv_imgproc -lopencv_highgui
