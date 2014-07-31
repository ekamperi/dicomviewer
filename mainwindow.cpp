#include <QDebug>
#include <QFileDialog>

#include "dicomfile.h"
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
    for (unsigned int i = 0; i < this->dicomWidgets.size(); i++) {
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
    qDebug() << Q_FUNC_INFO;

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
        /* Create a new GL Widget for every file/slice */
        MyGLWidget *pMyGLWidget = new MyGLWidget();

        /* Create a DicomFile object to dissect the input file */
        DicomFile *pDicomFile = new DicomFile();
        pDicomFile->setFileName(fileNames[i]);

        /* Extract the raw pixel data from the DICOM file */
        unsigned char *pRawPixelData =
                pDicomFile->getCompressedData();
        unsigned int width = pDicomFile->getWidth();
        unsigned int height = pDicomFile->getHeight();
        pMyGLWidget->loadTextureFile2(pRawPixelData, width, height);

        /* Add widget to UI */
        ui->verticalLayout->addWidget(pMyGLWidget);

        /* Add to list */
        this->dicomWidgets.push_back(pMyGLWidget);
    }
}
