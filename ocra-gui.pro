#-------------------------------------------------
#
# Project created by QtCreator 2016-10-28T15:08:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ocra-gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    taskwidget.cpp \
    gazebotools.cpp \
    physicstools.cpp \
    yarptools.cpp \
    connectionmonitor.cpp

HEADERS  += mainwindow.h \
    taskwidget.h \
    gazebotools.h \
    messagetypes.h \
    yarptools.h \
    physicstools.h \
    connectionmonitor.h

FORMS    += mainwindow.ui \
    taskwidget.ui

DISTFILES += \
    CMakeLists.txt
