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
    kinecthandler.cpp \
    kinectrunner.cpp \
    mainwindow.cpp \
    systeminputhandler.cpp

HEADERS += \
    kinectdetails.h \
    kinecthandler.h \
    kinectrunner.h \
    mainwindow.h \
    systeminputhandler.h

LIBS += -L"$$PWD/../../../../Program Files/Microsoft SDKs/Kinect/v1.7/lib/amd64/" -lKinect10
LIBS += -luser32

INCLUDEPATH += "$$PWD/../../../../Program Files/Microsoft SDKs/Kinect/v1.7/inc/"

FORMS += \
    mainwindow.ui
