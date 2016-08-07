#-------------------------------------------------
#
# Project created by QtCreator 2014-07-25T21:43:52
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = volmetrics
TEMPLATE = app

SOURCES += \
    helpers/dicomhelper.cpp \
    helpers/huconverter.cpp \
    helpers/mymath.cpp \
    widgets/ddtreewidget.cpp \
    widgets/examdetailswidget.cpp \
    widgets/gridwidget.cpp \
    widgets/imagewidget.cpp \
    widgets/myglwidget.cpp \
    widgets/patientexplorerwidget.cpp \
    widgets/slicewidget.cpp \
    widgets/startupmenu.cpp \
    widgets/topogram.cpp \
    widgets/workspacewidget.cpp \
    workers/finddicomworker.cpp \
    workers/loaddicomworker.cpp \
    denscircle.cpp \
    dicomfile.cpp \
    distline.cpp \
    examdetails.cpp \
    main.cpp \
    mainwindow.cpp \
    flowlayout.cpp \
    patient.cpp \
    patientexplorer.cpp \
    selectable.cpp \
    series.cpp \
    slice.cpp \
    study.cpp

HEADERS  += \
    include/helpers/dicomhelper.h \
    include/helpers/huconverter.h \
    include/helpers/mymath.h \
    include/widgets/ddtreewidget.h \
    include/widgets/examdetailswidget.h \
    include/widgets/gridwidget.h \
    include/widgets/imagewidget.h \
    include/widgets/myglwidget.h \
    include/widgets/patientexplorerwidget.h \
    include/widgets/slicewidget.h \
    include/widgets/startupmenu.h \
    include/widgets/topogram.h \
    include/widgets/workspacewidget.h \
    include/workers/finddicomworker.h \
    include/workers/loaddicomworker.h \
    denscircle.h \
    dicomfile.h \
    distline.h \
    examdetails.h \
    flowlayout.h \
    mainwindow.h \
    patient.h \
    patientexplorer.h \
    selectable.h \
    slice.h \
    series.h \
    study.h

FORMS += \
    ui/dicomfile.ui \
    ui/mainwindow.ui \
    ui/myglwidget.ui \
    ui/patientexplorerwidget.ui \
    ui/startupmenu.ui

unix|win32: LIBS += -ldcmdata -lofstd -ldcmimage -ldcmimgle -ldcmjpeg \
 -lGraphicsMagick++ -lGraphicsMagick -ljbig -llcms2 -ltiff -lfreetype -ljasper \
-ljpeg -lpng12 -lwmflite -lXext -lSM -lICE -lX11 -llzma -lbz2 -lxml2 -lz -lm \
-lgomp -lpthread

CONFIG += debug
#CONFIG  += qt warn_off release
#DEFINES += QT_NO_DEBUG_OUTPUT
#DEFINES += QT_NO_DEBUG

INCLUDEPATH+=/usr/include/ImageMagick

RESOURCES += ui/application.qrc
