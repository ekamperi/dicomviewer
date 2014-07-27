#-------------------------------------------------
#
# Project created by QtCreator 2014-07-25T21:43:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = volmetrics
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dicomviewer.cpp

HEADERS  += mainwindow.h \
    dicomviewer.h

FORMS    += mainwindow.ui \
    dicomviewer.ui

unix|win32: LIBS += -ldcmdata -lofstd
