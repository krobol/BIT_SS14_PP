#-------------------------------------------------
#
# Project created by QtCreator 2014-06-12T13:44:00
#
#-------------------------------------------------
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpticalFlow
TEMPLATE = app

INCLUDEPATH += "../../Dependencies/OpenCV/include/"
LIBS += -L"../../Dependencies/OpenCV/bin/"

build_pass:CONFIG(debug, debug|release){
    OBJECTS_DIR = build/debug
    LIBS += -L"../../Dependencies/OpenCV/lib/" \
    -lopencv_contrib249d\
    -lopencv_core249d\
    -lopencv_features2d249d\
    -lopencv_flann249d\
    -lopencv_gpu249d\
    -lopencv_highgui249d\
    -lopencv_imgproc249d\
    -lopencv_legacy249d\
    -lopencv_ml249d\
    -lopencv_nonfree249d\
    -lopencv_objdetect249d\
    -lopencv_photo249d\
    -lopencv_stitching249d\
    -lopencv_ts249d\
    -lopencv_video249d\
    -lopencv_videostab249d\
} else{
    OBJECTS_DIR = build/release
    LIBS += -L"../../Dependencies/OpenCV/lib/" \
    -lopencv_contrib249\
    -lopencv_core249\
    -lopencv_features2d249\
    -lopencv_flann249\
    -lopencv_gpu249\
    -lopencv_highgui249\
    -lopencv_imgproc249\
    -lopencv_legacy249\
    -lopencv_ml249\
    -lopencv_nonfree249\
    -lopencv_objdetect249\
    -lopencv_photo249\
    -lopencv_stitching249\
    -lopencv_ts249\
    -lopencv_video249\
    -lopencv_videostab249
}

SOURCES +=  main.cpp\
            mainwindow.cpp \
            playercontrols.cpp \
    opticalflowhornschunck.cpp \
    configdialog.cpp \
    opticalflowclg.cpp \
    opticalflowlucaskanade.cpp \
    opticalflowclgpyramidal.cpp \
    opticalflowlucaskanadepyramidal.cpp

HEADERS  += mainwindow.h \
            playercontrols.h \
            iopticalflowalgorithm.h \
            opticalflowhornschunck.h \
            configdialog.h \
            AlgorithmConfig.h \
    opticalflowclg.h \
    opticalflowlucaskanade.h \
    opticalflowclgpyramidal.h \
    opticalflowlucaskanadepyramidal.h
