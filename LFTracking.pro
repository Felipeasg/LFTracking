#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T21:05:31
#
#-------------------------------------------------

QT       += core gui opengl serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LFTracking
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    buffer.cpp \
    serialportsettingsdialog.cpp \
    serialportsettingsconfigset.cpp \
    serialportsettingmodel.cpp \
    qcustomplot.cpp \
    receivedata.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    buffer.h \
    serialportsettingsdialog.h \
    serialportsettingsconfigset.h \
    serialportsettingmodel.h \
    qcustomplot.h \
    receivedata.h

FORMS    += mainwindow.ui \
    serialportsettingsdialog.ui

RESOURCES += \
    res.qrc
