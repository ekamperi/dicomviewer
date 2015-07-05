#-------------------------------------------------
#
# Project created by QtCreator 2014-07-25T21:43:52
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = volmetrics
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    myglwidget.cpp \
    dicomfile.cpp \
    loaddicomworker.cpp \
    slice.cpp \
    examdetails.cpp \
    flowlayout.cpp \
    huconverter.cpp \
    myimagewidget.cpp \
    topogram.cpp \
    mymath.cpp \
    startupmenu.cpp \
    distline.cpp \
    selectable.cpp \
    denscircle.cpp \
    patientexplorer.cpp \
    imageseries.cpp \
    study.cpp \
    dicomhelper.cpp \
    patientexplorerwidget.cpp \
    finddicomworker.cpp \
    series.cpp \
    patient.cpp \
    gridwidget.cpp

HEADERS  += mainwindow.h \
    myglwidget.h \
    dicomfile.h \
    loaddicomworker.h \
    slice.h \
    examdetails.h \
    flowlayout.h \
    huconverter.h \
    myimagewidget.h \
    topogram.h \
    mymath.h \
    startupmenu.h \
    distline.h \
    selectable.h \
    denscircle.h \
    patientexplorer.h \
    imageseries.h \
    study.h \
    dicomhelper.h \
    patientexplorerwidget.h \
    finddicomworker.h \
    series.h \
    patient.h \
    gridwidget.h

FORMS    += mainwindow.ui \
    myglwidget.ui \
    dicomfile.ui \
    startupmenu.ui \
    patientexplorerwidget.ui

unix|win32: LIBS += -ldcmdata -lofstd -ldcmimage -ldcmimgle -ldcmjpeg \
 -lGraphicsMagick++ -lGraphicsMagick -ljbig -llcms2 -ltiff -lfreetype -ljasper \
-ljpeg -lpng12 -lwmflite -lXext -lSM -lICE -lX11 -llzma -lbz2 -lxml2 -lz -lm -lgomp -lpthread

CONFIG += debug
#CONFIG  += qt warn_off release
#DEFINES += QT_NO_DEBUG_OUTPUT
#DEFINES += QT_NO_DEBUG

INCLUDEPATH+=/usr/include/ImageMagick

