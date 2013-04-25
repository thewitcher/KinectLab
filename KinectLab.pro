#-------------------------------------------------
#
# Project created by QtCreator 2013-04-24T10:53:09
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = KinectLab
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    kinect-handler.cpp

HEADERS += \
    kinect-handler.h

LIBS += -L"$$PWD/../../../../Program Files/Microsoft SDKs/Kinect/v1.7/lib/amd64/" -lKinect10
LIBS += -luser32

INCLUDEPATH += "$$PWD/../../../../Program Files/Microsoft SDKs/Kinect/v1.7/inc/"
