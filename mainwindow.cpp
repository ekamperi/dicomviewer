#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
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
    Magick::InitializeMagick(NULL);

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

    this->setCursor(Qt::WaitCursor);

    int howMany = (int)this->slices.size();
    for (int i = 0; i < howMany; i++) {
        Slice *pSlice = this->slices.at(i);
        MyGLWidget *pMyGLWidget = new MyGLWidget();
        pSlice->setGLWidget(pMyGLWidget);
        pMyGLWidget->setSlice(pSlice);

        /* Set a minimum size on the widget or else it will be squeezed to
         * fit into the container and the scrollbars won't appear!
         * XXX: Minimum size should be calculated based on the application
         * size, no ?
         */
        pMyGLWidget->setMinimumSize(256, 256);

        /* XXX */
        connect(pMyGLWidget, SIGNAL(sliceDoubleClicked(Slice*)),
                this, SLOT(sliceDoubleClicked(Slice*)));

        /* Add the slice to the grid layout */
        this->flowLayout->addWidget(pMyGLWidget);
    }
    containerWidget->setLayout(flowLayout);

    this->setCursor(Qt::ArrowCursor);

    this->statusBar()->showMessage(
                QString::number(this->slices.size()) +
                " files were loaded succesfully.");
}

void MainWindow::on_actionExit_triggered()
{
    qDebug() << Q_FUNC_INFO;

    QCoreApplication::exit(0);
}

void MainWindow::on_actionClose_triggered()
{
    qDebug() << Q_FUNC_INFO;

    /* Check whether we are returning from full screen */
    QWidget *ww = ui->stackedWidget->currentWidget();
    if (ui->stackedWidget->currentWidget() == this->containerWidget2) {
        ui->stackedWidget->setCurrentWidget(this->containerWidget);
    } else {
        /* Remove all GL widgets from the flow layout */
        QLayoutItem *pLayoutItem;
        while (pLayoutItem = this->flowLayout->takeAt(0)) {
            delete pLayoutItem->widget();
        }

        /* Also remove the slices */
        std::vector<Slice *>::iterator it;
        for (it = slices.begin(); it != slices.end(); it++) {
            delete *it;
        }
        slices.clear();
    }
    this->statusBar()->showMessage("Ready.");
}

void MainWindow::sliceDoubleClicked(Slice *pSlice)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Slice " << pSlice->getIndex() << " was double clicked";

    MyGLWidget *pMyGLWidget = new MyGLWidget();
    pMyGLWidget->setSlice(pSlice);
    pMyGLWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pMyGLWidget->update();
    pMyGLWidget->show();

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
        qDebug() << Q_FUNC_INFO;
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
        }
    }

    return QWidget::event(pEvent);
}

void MainWindow::selectAllSlices(void)
{
    qDebug() << Q_FUNC_INFO;

    std::vector<Slice *>::iterator it;
    for (it = slices.begin(); it != slices.end(); it++) {
        Slice *pSlice = *it;
        bool isSelected = pSlice->isSelected();
        pSlice->setSelected(!isSelected);
        pSlice->getGLWidget()->update();
    }
}

void MainWindow::gotoNextSlice()
{
    qDebug() << Q_FUNC_INFO;

    this->gotoSlice(SliceDirection::Next);

}

void MainWindow::gotoPrevSlice()
{
    qDebug() << Q_FUNC_INFO;

    this->gotoSlice(SliceDirection::Prev);
}

void MainWindow::gotoSlice(SliceDirection::is dir)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "WTF...";
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
        /* We have reached the end */
        if (idx == slices.size()-1) {
            return;
        }
        pMyGLWidget->setSlice(slices[idx + 1]);
    } else {
        /* We heave reached the beginning */
        if (idx == 0) {
            return;
        }
        pMyGLWidget->setSlice(slices[idx - 1]);
    }

    pMyGLWidget->update();
    pLayout->update();
    containerWidget2->update();
    updateStatusBarForSlice();

    qDebug() << Q_FUNC_INFO;
}

void MainWindow::updateStatusBarForSlice(void) const
{
    qDebug() << Q_FUNC_INFO;

    QLayout *pLayout = containerWidget2->layout();
    Q_ASSERT(pLayout);
    MyGLWidget *pMyGLWidget = (MyGLWidget *)
            pLayout->itemAt(0)->widget();
    unsigned int idx = pMyGLWidget->getSliceIndex() + 1;
    qDebug() << "idx = " << idx;

    this->statusBar()->showMessage(
                QString("Slice: %1").arg(idx));
}

void MainWindow::wheelEvent(QWheelEvent *pEvent)
{
    qDebug() << Q_FUNC_INFO;

    int delta = pEvent->delta();
    if (delta > 0) {
        this->gotoPrevSlice();
    } else {
        this->gotoNextSlice();
    }

    pEvent->accept();
}
