#-------------------------------------------------
#
# Project created by QtCreator 2013-04-24T10:53:09
#
#-------------------------------------------------

QT       += core gui

CONFIG   += thread

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KinectLab

TEMPLATE = app


SOURCES += main.cpp \
    kinect-handler.cpp \
    main-window.cpp \
    kinect-runner.cpp

HEADERS += \
    kinect-handler.h \
    main-window.h \
    kinect-runner.h

LIBS += -L"$$PWD/../../../../Program Files/Microsoft SDKs/Kinect/v1.7/lib/amd64/" -lKinect10
LIBS += -luser32

INCLUDEPATH += "$$PWD/../../../../Program Files/Microsoft SDKs/Kinect/v1.7/inc/"

FORMS += \
    main-window.ui
