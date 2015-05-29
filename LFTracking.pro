#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T21:05:31
#
#-------------------------------------------------

QT       += core gui opengl serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LFTracking
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    mythread.cpp \
    buffer.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    mythread.h \
    buffer.h

FORMS    += mainwindow.ui
