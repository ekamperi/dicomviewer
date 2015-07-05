#include <QActionGroup>
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
#include "patientexplorerwidget.h"
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

    this->gridWidget = new GridWidget;
    this->containerWidget2 = new QWidget;
    this->pStartupMenu = new StartupMenu;
    ui->stackedWidget->addWidget(gridWidget);
    ui->stackedWidget->addWidget(containerWidget2);
    ui->stackedWidget->addWidget(this->pStartupMenu);
    ui->stackedWidget->setCurrentWidget(this->pStartupMenu);

    /* Connect signals from the startup menu widget to the main window */
    connect(this->pStartupMenu, SIGNAL(openDICOM_files()),
            this, SLOT(on_actionOpenDICOM_triggered()));
    connect(this->pStartupMenu, SIGNAL(openDICOM_dir()),
            this, SLOT(on_actionOpen_DICOM_dir_triggered()));
    connect(this->pStartupMenu, SIGNAL(openPatientExplorer()),
            this, SLOT(on_actionOpen_patient_explorer_triggered()));

    this->pGLWidget = new MyGLWidget();
    QVBoxLayout *pLayout = new QVBoxLayout();
    pLayout->addWidget(this->pGLWidget);
    containerWidget2->setLayout(pLayout);
    connect(this->pGLWidget, SIGNAL(sliceChanged(int)),
            this, SLOT(gotoSlice(int)));

    /* The first time ::statusBar() is called, it creates a status bar. */
    this->statusBar();
    this->statusBar()->showMessage("Ready.");

    /* XXX: This is a temporary hack or else Unity won't show any menu
     * at all (if the indicator-appmenus is removed.
     */
    this->menuBar()->setNativeMenuBar(false);

    /* Alignment groups (i.e., mutually exlusive checkable menu items) */
    /* Window/Width menu items (bone, head, lung, mediastinum, etc) */
    QActionGroup *pAlGroupWindowLevel = new QActionGroup(this);
    Q_ASSERT(pAlGroupWindowLevel);
    pAlGroupWindowLevel->addAction(ui->actionAbdomen);
    pAlGroupWindowLevel->addAction(ui->actionBone);
    pAlGroupWindowLevel->addAction(ui->actionHead);
    pAlGroupWindowLevel->addAction(ui->actionLung);
    pAlGroupWindowLevel->addAction(ui->actionMediastinum);

    /* Flip horizontally/vertically */
    QActionGroup *pAlGroupFlip = new QActionGroup(this);
    Q_ASSERT(pAlGroupFlip);
    pAlGroupFlip->addAction(ui->actionFlip_Horizontally);
    pAlGroupFlip->addAction(ui->actionFlip_Vertically);

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
    delete this->gridWidget;
    delete this->containerWidget2;

    /* Deregister decompression codecs */
    DcmRLEDecoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(
                NULL,
                "About", "Volmetrics development branch.\n"
                "(c) Stathis Kamperis, Eleni-Maria Stea, 2014-2015.");
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

    this->loadDCMFiles(fileNames);
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

    int howMany = (int)this->vecSlices.size();
    float maxPixel = -1.0;

    /* Keep track of maximum pixel value acros all slices */
    for (int i = 0; i < howMany; i++) {
        Slice *pSlice = this->vecSlices.at(i);
        Q_ASSERT(pSlice);
        float val = pSlice->getMaxPixel();
        if (val > maxPixel) {
            maxPixel = val;
        }
    }

    /* Normalize CT values across all slices */
    for (int i = 0; i < howMany; i++) {
        Slice *pSlice = this->vecSlices.at(i);
        Q_ASSERT(pSlice);
        pSlice->normalizePixels(maxPixel);
    }

    /* Create a grid with the slices as thumbnail images */
    connect(this->gridWidget, SIGNAL(sliceDoubleClicked(const Slice*)),
            this, SLOT(gotoSlice(const Slice*)));
    this->gridWidget->addSlices(this->vecSlices);
    ui->stackedWidget->setCurrentWidget(gridWidget);

    this->setCursor(Qt::ArrowCursor);

    /* Load the slices to gpu */
    this->pGLWidget->loadSlices(this->vecSlices);

    this->statusBar()->showMessage(
                QString::number(this->vecSlices.size()) +
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
        ui->stackedWidget->setCurrentWidget(this->gridWidget);
    } else {
        /* Remove all GL widgets from the flow layout */
        QLayoutItem *pLayoutItem;
//        while ((pLayoutItem = this->flowLayout->takeAt(0)) != NULL) {
//            delete pLayoutItem->widget();
//        }

        /* Also remove the slices */
        QVector<Slice *>::iterator it;
        for (it = vecSlices.begin(); it != vecSlices.end(); it++) {
            delete *it;
        }
        vecSlices.clear();
        ui->stackedWidget->setCurrentWidget(this->pStartupMenu);
    }
    ui->stackedWidget->update();
    this->statusBar()->showMessage("Ready.");
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
            gotoSlice(vecSlices.size() - 1);
        }
    }

    return QWidget::event(pEvent);
}

void MainWindow::selectAllSlices(void)
{
    QVector<Slice *>::iterator it;
    for (it = vecSlices.begin(); it != vecSlices.end(); it++) {
        Slice *pSlice = *it;
        Q_ASSERT(pSlice);
        bool isSelected = pSlice->isSelected();
        pSlice->setSelected(!isSelected);
        pSlice->getImageWidget()->update();
    }
}

void MainWindow::gotoNextSlice(void)
{
    this->gotoSlice(SliceDirection::Next);
}

void MainWindow::gotoPrevSlice(void)
{
    this->gotoSlice(SliceDirection::Prev);
}

void MainWindow::gotoSlice(SliceDirection::is dir)
{
    Q_ASSERT(dir == SliceDirection::Prev || dir == SliceDirection::Next);

    unsigned int idx = this->pGLWidget->getSliceIndex();

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
        idx = vecSlices.size() - 1;
    } else if (idx > vecSlices.size()-1) {
        idx = 0;
    }

    /* Set new slice */
    ui->stackedWidget->setCurrentWidget(containerWidget2);
    this->pGLWidget->setSlice(this->vecSlices[idx]);

    /* Update the status bar accordingly */
    updateStatusBarForSlice();
}

void MainWindow::gotoSlice(const Slice *pSlice)
{
    Q_ASSERT(pSlice);
    gotoSlice(pSlice->getIndex());
}

void MainWindow::updateStatusBarForSlice(void) const
{
    QLayout *pLayout = containerWidget2->layout();
    Q_ASSERT(pLayout);
    MyGLWidget *pMyGLWidget = (MyGLWidget *)
            pLayout->itemAt(0)->widget();
    unsigned int idx = pMyGLWidget->getSliceIndex() + 1;

    this->statusBar()->showMessage(
                QString("Slice: %1 / %2").arg(idx).arg(vecSlices.size()));
}

void MainWindow::wheelEvent(QWheelEvent *pEvent)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        pEvent->ignore();
    } else {
        pEvent->accept();
        int delta = pEvent->delta();
        if (delta > 0) {
            this->gotoPrevSlice();
        } else {
            this->gotoNextSlice();
        }
    }
}

void MainWindow::on_actionDistance_triggered()
{
    qDebug() << Q_FUNC_INFO;
    bool flag = this->pGLWidget->isDistanceMeasureEnabled();
    this->pGLWidget->setDistanceMeasure(!flag);  // toggle
}

void MainWindow::on_actionDensity_HUs_triggered()
{
    qDebug() << Q_FUNC_INFO;
    bool flag = this->pGLWidget->isDensityMeasureEnabled();
    this->pGLWidget->setDensityMeasure(!flag);  // toggle
}

void MainWindow::on_actionAbdomen_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->getSlice()->setWindow(HUWindows::ABDOMEN);
}

void MainWindow::on_actionBone_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->getSlice()->setWindow(HUWindows::BONE);
}

void MainWindow::on_actionLung_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->getSlice()->setWindow(HUWindows::LUNG);
}

void MainWindow::on_actionHead_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->getSlice()->setWindow(HUWindows::HEAD);
}

void MainWindow::on_actionMediastinum_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->getSlice()->setWindow(HUWindows::MEDIASTINUM);
}

void MainWindow::on_actionFlip_Horizontally_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->setGeomTransformation(Geometry::FLIP_HORIZONTALLY);
}

void MainWindow::on_actionFlip_Vertically_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->setGeomTransformation(Geometry::FLIP_VERTICALLY);
}

void MainWindow::on_actionTopogram_triggered()
{
    qDebug() << Q_FUNC_INFO;
#define PI 3.1415926
    this->pGLWidget->genTopogram(0*PI/180.0);
}

void MainWindow::on_actionOpen_DICOM_dir_triggered()
{
    qDebug() << Q_FUNC_INFO;

    /* Select DICOM directory to open */
    QString dir = QFileDialog::getExistingDirectory(
                this, "Open Directory",
             QDir::homePath(),
             QFileDialog::ShowDirsOnly
           | QFileDialog::DontResolveSymlinks);

    /* If user clicked cancel, just return */
    if (dir.isEmpty()) {
        qDebug() << "dir is empty!";
        return;
    }

    /* Find all the .dcm files in the selected directory */
    QStringList nameFilter("*.dcm");
    QDir directory(dir);
    QStringList res = directory.entryList(nameFilter);

    for (int count = 0; count < res.size (); count ++) {
            res[count].prepend (dir + "/");
        }

    this->loadDCMFiles(res);
}

void MainWindow::loadDCMFiles(QStringList fileNames)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << fileNames;

    this->progressDialog = new QProgressDialog(
                "Loading DICOM files...",
                "Abort operation", 1, fileNames.size(), this);
    this->progressDialog->setWindowModality(Qt::WindowModal);
    this->progressDialog->show();

    loadDicomThread = new LoadDicomThread(fileNames, &vecSlices, this);
    Q_ASSERT(loadDicomThread);

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

void MainWindow::on_actionPan_triggered()
{
    qDebug() << Q_FUNC_INFO;
    bool flag = this->pGLWidget->isPanMode();
    this->pGLWidget->setPanMode(!flag);  // toggle
}

void MainWindow::on_action_Reset_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->resetView();
}

void MainWindow::on_actionDeleteAllMeasures_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->deleteAllMeasures();
}

void MainWindow::on_actionDeleteSelectedMeasures_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pGLWidget->deleteSelectedMeasures();
}

void MainWindow::on_actionOpen_patient_explorer_triggered()
{
    qDebug() << Q_FUNC_INFO;
    PatientExplorerWidget *pew = new PatientExplorerWidget();
    pew->show();
}
