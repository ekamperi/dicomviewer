#include <QDebug>
#include <QFileDialog>
#include <QtGlobal>
#include <QProgressDialog>

#include "dicomfile.h"
#include "loaddicomworker.h"
#include "mainwindow.h"
#include "myglwidget.h"
#include "ui_mainwindow.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

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

    this->progressDialog = new QProgressDialog(
                "Loading DICOM files...",
                "Abort operation", 0, fileNames.size(), this);
    this->progressDialog->setWindowModality(Qt::WindowModal);
    this->progressDialog->show();

    loadDicomThread = new LoadDicomThread(fileNames, &slices, this);
    connect(loadDicomThread, SIGNAL(finished()),
            loadDicomThread, SLOT(deleteLater()));
    connect(loadDicomThread, SIGNAL(finished()),
            this, SLOT(filesLoaded()));
    connect(loadDicomThread, SIGNAL(reportProgress(unsigned int)),
            this, SLOT(getProgress(unsigned int)));
    connect(progressDialog, SIGNAL(canceled()),
            this, SLOT(progressDialogCanceled()));

    loadDicomThread->start();
}

void MainWindow::getProgress(unsigned int cnt)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "cnt =" << cnt;

    if (!this->progressDialog->wasCanceled()) {
        this->progressDialog->setValue(cnt + 1);
    }
}

void MainWindow::progressDialogCanceled()
{
    qDebug() << Q_FUNC_INFO;

    loadDicomThread->abortOperation();
}

void MainWindow::filesLoaded()
{
    qDebug() << Q_FUNC_INFO;

    /* Add widget to UI */
    qDebug() << this->slices.size();

    for (int i = 0; i < qMin((int)this->slices.size(), 4); i++) {
         MyGLWidget *pMyGLWidget = new MyGLWidget();
         Slice *s = this->slices.at(i);
         pMyGLWidget->loadTextureFile2(
                     s->getRawPixelData(),
                     s->getWidth(),
                     s->getHeight());
         ui->verticalLayout->addWidget(pMyGLWidget);
    }
}
