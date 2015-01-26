#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QtGlobal>
#include <QProgressDialog>

#include "dicomfile.h"
#include "loaddicomworker.h"
#include "mainwindow.h"
#include "myglwidget.h"
#include "myimagewidget.h"
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

    this->scrollArea = new QScrollArea;
    this->flowLayout = new FlowLayout;
    this->containerWidget = new QWidget;
    this->containerWidget2 = new QWidget;
    ui->stackedWidget->addWidget(containerWidget);
    ui->stackedWidget->addWidget(containerWidget2);
    ui->stackedWidget->setCurrentWidget(containerWidget);

    /* The first time ::statusBar() is called, it creates a status bar. */
    this->statusBar();
    this->statusBar()->showMessage("Ready.");

    /* XXX: This is a temporary hack or else Unity won't show any menu
     * at all (if the indicator-appmenus is removed.
     */
    this->menuBar()->setNativeMenuBar(false);

    /*
     * Initialize library or else it will abort.
     * It may not be needed in newer versions of it.
     */
    //Magick::InitializeMagick(NULL);

    /* Register decompression codecs */
    DcmRLEDecoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs();
}

MainWindow::~MainWindow()
{
    /*
     * There is no need to manually delete the dynamically allocated
     * MyGLWidget's. Qt will do that for us when the parent layout will
     * be destroyed!
     */
    delete ui;
    delete this->flowLayout;
    delete this->containerWidget;
    delete this->containerWidget2;

    /* Deregister decompression codecs */
    DcmRLEDecoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();
}

void MainWindow::on_actionAbout_triggered()
{
}

void MainWindow::on_actionOpenDICOM_triggered()
{
    /* Select DICOM files to open */
    QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(trUtf8("DICOM files (*.dcm)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
    }

    /* If user clicked cancel, just return */
    if (fileNames.isEmpty()) {
        qDebug() << "fileNames is empty!";
        return;
    }

    this->progressDialog = new QProgressDialog(
                "Loading DICOM files...",
                "Abort operation", 1, fileNames.size(), this);
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
    if (!this->progressDialog->wasCanceled()) {
        this->progressDialog->setValue(cnt + 1);
    }
}

void MainWindow::progressDialogCanceled()
{
    loadDicomThread->abortOperation();
}

void MainWindow::filesLoaded()
{
    this->setCursor(Qt::WaitCursor);

    int howMany = (int)this->slices.size();
    float maxPixel = -1.0;

    for (unsigned int i = 0; i < howMany; i++) {
        Slice *pSlice = this->slices.at(i);
        Q_ASSERT(pSlice);

        float val = pSlice->getMaxPixel();
        if (val > maxPixel) {
            maxPixel = val;
        }

        MyImageWidget *pMyImageWidget = new MyImageWidget();
        pSlice->setImageWidget(pMyImageWidget);
        pMyImageWidget->setSlice(pSlice);

        /* XXX */
        connect(pMyImageWidget, SIGNAL(sliceDoubleClicked(Slice*)),
                this, SLOT(sliceDoubleClicked(Slice*)));

        pMyImageWidget->setMinimumSize(256, 256);

        /* Add the slice to the grid layout */
        this->flowLayout->addWidget(pMyImageWidget);
    }

    /* Normalize CT values across all slices */
    for (unsigned int i = 0; i < howMany; i++) {
        Slice *pSlice = this->slices.at(i);
        Q_ASSERT(pSlice);
        pSlice->normalizePixels(maxPixel);
    }

    /* Add a default window/width */

    containerWidget->setLayout(this->flowLayout);
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->stackedWidget->addWidget(scrollArea);
    ui->stackedWidget->setCurrentWidget(scrollArea);

    this->setCursor(Qt::ArrowCursor);

    this->statusBar()->showMessage(
                QString::number(this->slices.size()) +
                " files were loaded succesfully.");
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::exit(0);
}

void MainWindow::on_actionClose_triggered()
{
    /* Check whether we are returning from full screen */
    if (ui->stackedWidget->currentWidget() == this->containerWidget2) {
        ui->stackedWidget->setCurrentWidget(this->scrollArea);
    } else {
        /* Remove all GL widgets from the flow layout */
        QLayoutItem *pLayoutItem;
        while ((pLayoutItem = this->flowLayout->takeAt(0)) != NULL) {
            delete pLayoutItem->widget();
        }

        /* Also remove the slices */
        std::vector<Slice *>::iterator it;
        for (it = slices.begin(); it != slices.end(); it++) {
            delete *it;
        }
        slices.clear();
    }
    ui->stackedWidget->update();
    this->statusBar()->showMessage("Ready.");
}

void MainWindow::sliceDoubleClicked(Slice *pSlice)
{
    MyGLWidget *pMyGLWidget = new MyGLWidget();
    pMyGLWidget->setSlice(pSlice);
    pMyGLWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pMyGLWidget->update();

    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setContentsMargins(QMargins(0,0,0,0));
    verticalLayout->addWidget(pMyGLWidget);
    verticalLayout->update();
    containerWidget2->setLayout(verticalLayout);

    ui->stackedWidget->setCurrentWidget(containerWidget2);

    updateStatusBarForSlice();
}

bool MainWindow::event(QEvent *pEvent)
{
    if (pEvent->type() == QEvent::KeyPress) {
        QKeyEvent *pke = static_cast<QKeyEvent *>(pEvent);
        int key = pke->key();
        if (key == Qt::Key_Escape) {
            this->on_actionClose_triggered();
            return true;
        } else if (key == Qt::Key_A
                   && (QApplication::keyboardModifiers() & Qt::ControlModifier)) {
            selectAllSlices();
            return true;
        } else if (key == Qt::Key_PageUp) {
            gotoPrevSlice();
            return true;
        } else if (key == Qt::Key_PageDown || key == Qt::Key_Space) {
            gotoNextSlice();
            return true;
        } else if (key == Qt::Key_Home) {
            gotoSlice(0);
        } else if (key == Qt::Key_End) {
            gotoSlice(slices.size() - 1);
        }
    }

    return QWidget::event(pEvent);
}

void MainWindow::selectAllSlices(void)
{
    std::vector<Slice *>::iterator it;
    for (it = slices.begin(); it != slices.end(); it++) {
        Slice *pSlice = *it;
        bool isSelected = pSlice->isSelected();
        pSlice->setSelected(!isSelected);
        pSlice->getImageWidget()->update();
    }
}

void MainWindow::gotoNextSlice()
{
    this->gotoSlice(SliceDirection::Next);
}

void MainWindow::gotoPrevSlice()
{
    this->gotoSlice(SliceDirection::Prev);
}

void MainWindow::gotoSlice(SliceDirection::is dir)
{
    Q_ASSERT(dir == SliceDirection::Prev || dir == SliceDirection::Next);

    /* Get current slice and index */
    QLayout *pLayout = containerWidget2->layout();
    if (!pLayout) {
        qDebug() << "pLayout is NULL! returning! (no worries)";
        return;
    }
    MyGLWidget *pMyGLWidget = (MyGLWidget *)pLayout->itemAt(0)->widget();
    Q_ASSERT(pMyGLWidget);
    unsigned int idx = pMyGLWidget->getSliceIndex();

    if (dir == SliceDirection::Next) {
        gotoSlice(idx+1);
    } else {
        gotoSlice(idx-1);
    }
}

void MainWindow::gotoSlice(int idx)
{
    /* Check whether we are inside the bounds or we are recycling */
    if (idx < 0) {
        idx = slices.size() - 1;
    } else if (idx > slices.size()-1) {
        idx = 0;
    }

    /* Get current slice and index */
    QLayout *pLayout = containerWidget2->layout();
    if (!pLayout) {
        qDebug() << "pLayout is NULL! returning! (no worries)";
        return;
    }

    /* Set new slice */
    MyGLWidget *pMyGLWidget = (MyGLWidget *)pLayout->itemAt(0)->widget();
    Q_ASSERT(pMyGLWidget);
    pMyGLWidget->setSlice(slices[idx]);

    /* Update the status bar accordingly */
    updateStatusBarForSlice();
}

void MainWindow::updateStatusBarForSlice(void) const
{
    QLayout *pLayout = containerWidget2->layout();
    Q_ASSERT(pLayout);
    MyGLWidget *pMyGLWidget = (MyGLWidget *)
            pLayout->itemAt(0)->widget();
    unsigned int idx = pMyGLWidget->getSliceIndex() + 1;

    this->statusBar()->showMessage(
                QString("Slice: %1 / %2").arg(idx).arg(slices.size()));
}

void MainWindow::wheelEvent(QWheelEvent *pEvent)
{
    int delta = pEvent->delta();
    if (delta > 0) {
        this->gotoPrevSlice();
    } else {
        this->gotoNextSlice();
    }

    pEvent->accept();
}
