#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dicomviewer.h"


/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimage/diregist.h"   // Support for color images
#include "dcmtk/dcmdata/dcrledrg.h"    // Support for RLE images
#include "dcmtk/dcmjpeg/djdecode.h"    // Support for JPEG images

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Register decompression codecs */
    DcmRLEDecoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs();

    /* */
    this->pScene = new QGraphicsScene();
    this->pView = new QGraphicsView(pScene);
}

MainWindow::~MainWindow()
{
    delete this->pView;
    delete this->pScene;
    delete ui;

    /* Deregister decompression codecs */
    DcmRLEDecoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();
}

void MainWindow::on_actionAbout_triggered()
{
}

void MainWindow::on_actionOpenDICOM_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this);

    DicomViewer *pDicomViewer = new DicomViewer;
    pDicomViewer->loadDicomFile(filename);
    pDicomViewer->setAttribute(Qt::WA_DeleteOnClose);
    pDicomViewer->show();
}
