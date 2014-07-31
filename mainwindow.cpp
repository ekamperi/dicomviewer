#include <QDebug>
#include <QFileDialog>

#include "dicomviewer.h"
#include "mainwindow.h"
#include "myglwidget.h"
#include "ui_mainwindow.h"

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

    /*
     * Initialize library or else it will abort.
     * It may not be needed in newer versions of it.
     */
    Magick::InitializeMagick(NULL);



    /* Register decompression codecs */
    DcmRLEDecoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs();
}

MainWindow::~MainWindow()
{
    for (int i = 0; i < this->dicomWidgets.size(); i++) {
        delete this->dicomWidgets[i];
    }
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
    QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(trUtf8("DICOM files (*.dcm)"));
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

//    DicomViewer *pDicomViewer = new DicomViewer;
//    pDicomViewer->loadDicomFile(filename);
//    pDicomViewer->setAttribute(Qt::WA_DeleteOnClose);
//    pDicomViewer->show();

    for (int i = 0; i < fileNames.size(); i++) {
        this->dicomWidgets.push_back(new MyGLWidget());
        this->dicomWidgets[i]->loadTextureFile("/home/stathis/Desktop/audit1.png");
        ui->verticalLayout->addWidget(this->dicomWidgets[i]);
    }
}
