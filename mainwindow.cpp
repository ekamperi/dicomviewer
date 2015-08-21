#include <QActionGroup>
#include <QtAlgorithms>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QtGlobal>
#include <QProgressDialog>

#include "dicomfile.h"
#include "include/workers/loaddicomworker.h"
#include "mainwindow.h"
#include "include/widgets/imagewidget.h"
#include "include/helpers/huconverter.h"
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

    /* Create the various widgets and populate the stacked widget (the stacked
     * widget will only show 1 widget at a time */
    this->pGridWidget = new GridWidget;
    this->pSliceWidget = new SliceWidget(&this->vecSlices);
    this->pStartupMenu = new StartupMenu;
    ui->stackedWidget->addWidget(this->pGridWidget);
    ui->stackedWidget->addWidget(this->pSliceWidget);
    ui->stackedWidget->addWidget(this->pStartupMenu);
    ui->stackedWidget->setCurrentWidget(this->pStartupMenu);

    /* The patient explorer widget is about searching patient studies in the media.
     * It is also used as a case explorer when docked in the main window.
     */
    this->pExplorerWidget = new PatientExplorerWidget();
    Q_ASSERT(this->pExplorerWidget);
    this->pDockWidget = NULL;

    /* Setup the main toolbar, but don't display it yet */
    this->setupToolbar();

    /* Guess what this does :) */
    this->connectSignals();

    /* Alignemnt groups are mutually exclusive checkable menu items */
    this->setupAlignmentGroups();

    /* The first time ::statusBar() is called, it creates a status bar. */
    this->statusBar();
    this->statusBar()->showMessage("Ready.");

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
    delete this->pGridWidget;
    delete this->pSliceWidget;
    delete this->pExplorerWidget;

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

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::exit(0);
}

void MainWindow::on_actionClose_triggered()
{
    /* Check whether we are returning from full screen */
    if (ui->stackedWidget->currentWidget() == this->pSliceWidget) {
        this->switchToGridWidget();
    } else {
        // XXX: cleanup
        this->switchToStartupMenu();
    }
    ui->stackedWidget->update();
    this->statusBar()->showMessage("Ready.");
}

void MainWindow::updateStatusBarForSlice(int idx) const
{
    int current = idx + 1;
    int total = this->vecSlices.size();
    this->statusBar()->showMessage(
                QString("Slice: %1 / %2").arg(current).arg(total));
}

/*******************************************************************************
 *                  TOOLS (Measure distance, etc)
 ******************************************************************************/
void MainWindow::on_actionDistance_triggered()
{
    qDebug() << Q_FUNC_INFO;
    bool flag = this->pSliceWidget->pGLWidget->isDistanceMeasureEnabled();
    this->pSliceWidget->pGLWidget->setDistanceMeasure(!flag);  // toggle
}

void MainWindow::on_actionDensity_HUs_triggered()
{
    qDebug() << Q_FUNC_INFO;
    bool flag = this->pSliceWidget->pGLWidget->isDensityMeasureEnabled();
    this->pSliceWidget->pGLWidget->setDensityMeasure(!flag);  // toggle
}

void MainWindow::on_actionDeleteAllMeasures_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pSliceWidget->pGLWidget->deleteAllMeasures();
}

void MainWindow::on_actionDeleteSelectedMeasures_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pSliceWidget->pGLWidget->deleteSelectedMeasures();
}

/*******************************************************************************
 *              WINDOW/WIDTH LEVELS (Abdomenm, Bones, Lung, etc)
 ******************************************************************************/
void MainWindow::on_actionAbdomen_triggered()
{
    qDebug() << Q_FUNC_INFO;
    emit this->windowChanged(HUWindows::ABDOMEN);
}

void MainWindow::on_actionBone_triggered()
{
    qDebug() << Q_FUNC_INFO;
    emit this->windowChanged(HUWindows::BONE);
}

void MainWindow::on_actionLung_triggered()
{
    qDebug() << Q_FUNC_INFO;
    emit this->windowChanged(HUWindows::LUNG);
}

void MainWindow::on_actionHead_triggered()
{
    qDebug() << Q_FUNC_INFO;
    emit this->windowChanged(HUWindows::HEAD);
}

void MainWindow::on_actionMediastinum_triggered()
{
    qDebug() << Q_FUNC_INFO;
    emit this->windowChanged(HUWindows::MEDIASTINUM);
}

void MainWindow::on_actionSoft_tissue_triggered()
{
    qDebug() << Q_FUNC_INFO;
    emit this->windowChanged(HUWindows::SOFT_TISSUE);
}

/*******************************************************************************
 *                      GEOMETRIC TRANSFORMATIONS
 ******************************************************************************/
void MainWindow::on_actionFlip_Horizontally_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pSliceWidget->pGLWidget->setGeomTransformation(Geometry::FLIP_HORIZONTALLY);
}

void MainWindow::on_actionFlip_Vertically_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pSliceWidget->pGLWidget->setGeomTransformation(Geometry::FLIP_VERTICALLY);
}

void MainWindow::on_actionPan_triggered()
{
    qDebug() << Q_FUNC_INFO;
    bool flag = this->pSliceWidget->pGLWidget->isPanMode();
    this->pSliceWidget->pGLWidget->setPanMode(!flag);  // toggle
}

void MainWindow::on_action_Reset_triggered()
{
    qDebug() << Q_FUNC_INFO;
    this->pSliceWidget->pGLWidget->resetView();
}

void MainWindow::on_actionTopogram_triggered()
{
    qDebug() << Q_FUNC_INFO;
#define PI 3.1415926
    this->pSliceWidget->pGLWidget->genTopogram(0*PI/180.0);
}

/*******************************************************************************
 *                      FILE OPERATIONS
 ******************************************************************************/
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

    this->loadDicomFiles(fileNames);
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

    this->loadDicomFiles(res);
}

void MainWindow::on_actionOpen_patient_explorer_triggered()
{
    qDebug() << Q_FUNC_INFO;

    if (this->pDockWidget) {
        this->pDockWidget->setVisible(true);
        return;
    }

    /* The dock widget is used to allowed patient explorer widget to be
     * embedded in the main window or float. Don't create it until the user
     * hits to open patient explorer widget, eitherwise an empty dock will be
     * created and shown to main window.
     */
    this->pDockWidget = new QDockWidget("Patients", this);
    Q_ASSERT(this->pDockWidget);

    this->pDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->pDockWidget->setFloating(true);     // By default, float!
    this->pDockWidget->setWidget(this->pExplorerWidget);
    this->pDockWidget->resize(800, 480);

    /* Connect the signals */
    connect(this->pDockWidget, SIGNAL(topLevelChanged(bool)),
            this->pExplorerWidget, SLOT(updateUI(bool)));
    addDockWidget(Qt::LeftDockWidgetArea, this->pDockWidget);

    /* For some reason in Ubuntu Linux the patient explorer widget is shown
     * outside and far away from main window, which is very annoying. Try to
     * center it. XXX: Check how this code affects Windows/Mac OSX and other
     * Linux distributions.
    */
    this->pDockWidget->move(
        this->frameGeometry().topLeft() +
        this->rect().center() - pDockWidget->rect().center());
}

void MainWindow::loadDicomFiles(QStringList fileNames)
{
    qDebug() << Q_FUNC_INFO;

    /* Create a progress dialog */
    this->progressDialog = new QProgressDialog(
                "Loading DICOM files...",
                "Abort operation", 1, fileNames.size(), this);
    this->progressDialog->setWindowModality(Qt::WindowModal);
    this->progressDialog->show();

    /* Create the worker thread */
    loadDicomThread = new LoadDicomThread(fileNames, &this->vecSlices, this);
    Q_ASSERT(loadDicomThread);

    /* Connect the signals */
    connect(loadDicomThread, SIGNAL(finished()),
            loadDicomThread, SLOT(deleteLater()));
    connect(loadDicomThread, SIGNAL(finished()),
            this, SLOT(filesLoaded()));
    connect(loadDicomThread, SIGNAL(reportProgress(unsigned int)),
            this, SLOT(getProgress(unsigned int)));
    connect(progressDialog, SIGNAL(canceled()),
            this, SLOT(progressDialogCanceled()));

    /* This doesn't block */
    loadDicomThread->start();
}

bool sliceComparator(const Slice *pLeft, const Slice *pRight)
{
    Q_ASSERT(pLeft);
    Q_ASSERT(pRight);
    int left  =  pLeft->getExamDetails().getSliceLocation().toInt();
    int right = pRight->getExamDetails().getSliceLocation().toInt();
    return left > right;    //XXX
}

void MainWindow::filesLoaded(void)
{
    qDebug() << Q_FUNC_INFO;

    /* This is going to take a while */
    this->displayWaitCursor();

    /* Reorder the slices based on their slice location, if any */
    // XXX skip if no Slice location available
    const int howMany = this->vecSlices.size();
    qSort(vecSlices.begin(), vecSlices.end(), sliceComparator);
    for (int i = 0; i < howMany; i++) {
        Slice *pSlice = this->vecSlices.at(i);
        Q_ASSERT(pSlice);
        pSlice->setIndex(i);
    }

    /* Keep track of maximum pixel value acros all slices */
    float maxPixel = -1.0;
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
    this->pGridWidget->addSlices(this->vecSlices);
    this->switchToGridWidget();

    /* Load the slices to gpu (XXX: this could be done upon double clicking the thumbnail) */
    this->pSliceWidget->pGLWidget->loadSlices(this->vecSlices);

    /* We are done */
    this->displayArrowCursor();
    this->statusBar()->showMessage(
                QString::number(this->vecSlices.size()) +
                " files were loaded succesfully.");
}

void MainWindow::gotoSlice(const Slice *pSlice)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "gotoSlice() ->" << pSlice->getIndex();

    this->pSliceWidget->pGLWidget->setSlice((Slice *)pSlice);

    /* Change view from the grid widget to the (single) slice widget */
    switchToSliceWidget();
}

/*******************************************************************************
 *                          SIGNALS CONNECTIONS
 ******************************************************************************/
void MainWindow::connectSignals(void) const
{
    qDebug() << Q_FUNC_INFO;

    /* Use a unique connection by default, as we may be calling this function
     * more than once
     */
    const Qt::ConnectionType connType = Qt::UniqueConnection;

    /* Connect signals from the startup menu widget to the main window */
    connect(this->pStartupMenu, SIGNAL(openDICOM_files()),
            this, SLOT(on_actionOpenDICOM_triggered()),
            connType);
    connect(this->pStartupMenu, SIGNAL(openDICOM_dir()),
            this, SLOT(on_actionOpen_DICOM_dir_triggered()),
            connType);
    connect(this->pStartupMenu, SIGNAL(openPatientExplorer()),
            this, SLOT(on_actionOpen_patient_explorer_triggered()),
            connType);
    connect(this->pStartupMenu, SIGNAL(patientDropped()),
            this->pExplorerWidget, SLOT(loadDroppedPatient()));

    /* Connect signals from the grid widget to the main window */
    connect(this->pGridWidget, SIGNAL(sliceDoubleClicked(const Slice *)),
            this, SLOT(gotoSlice(const Slice *)),
            connType);
    connect(this, SIGNAL(windowChanged(HUWindows::window)),
            this->pGridWidget, SLOT(changeWindow(HUWindows::window)),
            connType);
    connect(this->pGridWidget, SIGNAL(heavyTaskInitiated()),
            this, SLOT(displayWaitCursor()),
            connType);
    connect(this->pGridWidget, SIGNAL(heavyTaskCompleted()),
            this, SLOT(displayArrowCursor()),
            connType);

    /* Connect signals from the slice widget to the main window */
    connect(this, SIGNAL(windowChanged(HUWindows::window)),
            this->pSliceWidget, SLOT(changeWindow(HUWindows::window)),
            connType);
    connect(this->pSliceWidget, SIGNAL(sliceChanged(int)),
            this, SLOT(updateStatusBarForSlice(int)),
            connType);
    connect(this->pSliceWidget, SIGNAL(backToGridWidget()),
            this, SLOT(switchToGridWidget()),
            connType);

    /* Connect signals from the patient explorer widget to the main window */
    connect(this->pExplorerWidget, SIGNAL(loadSeries(const QList<QString> &)),
            this, SLOT(loadSeries(const QList<QString> &)),
            connType);
    connect(this->pExplorerWidget, SIGNAL(loadSeries(const QList<QString> &)),
            this, SLOT(switchToGridWidget()),
            connType);
}

/*******************************************************************************
 *                              ALIGNMENT GROUPS
 ******************************************************************************/
void MainWindow::setupAlignmentGroups(void)
{
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
}

/*******************************************************************************
 *                              VARIOUS STUFF
 ******************************************************************************/
void MainWindow::loadSeries(const QList<QString> &files)
{
    qDebug() << Q_FUNC_INFO;

    /* Remove the old widget-case */
    ui->stackedWidget->removeWidget(this->pGridWidget);
    this->pGridWidget->deleteLater();
    this->vecSlices.clear();

    /* And create a new one */
    this->pGridWidget = new GridWidget;
    Q_ASSERT(this->pGridWidget);
    this->connectSignals();
    ui->stackedWidget->addWidget(this->pGridWidget);

    /* Load the new files */
    this->loadDicomFiles(files);
}

void MainWindow::changeWindow(QString newWindow)
{
    qDebug() << Q_FUNC_INFO;

    /* Convert the text from Combo Box to appropriate HU window structure, e.g.
     * "ABDOMEN" -> HUWindows::ABDOMEN */
    HUWindows::window newHUWindow = HUWindow::fromText(newWindow);

    /* Emit a signal so that anyone interested (e.g. SliceWidget)
     * redraws itself. */
    emit this->windowChanged(newHUWindow);
}

/*******************************************************************************
*                       USER INTERFACE OPERATIONS
*******************************************************************************/

void MainWindow::switchToStartupMenu(void)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(this->pStartupMenu);
    ui->stackedWidget->setCurrentWidget(this->pStartupMenu);
    this->hideMainToolbar();

    /* Also, if there were a docked patient explorer widget, un-dock it */
    // XXX
}

void MainWindow::switchToGridWidget(void)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(this->pSliceWidget);
    ui->stackedWidget->setCurrentWidget(this->pGridWidget);
    this->showMainToolbar();
}

void MainWindow::switchToSliceWidget(void)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(this->pSliceWidget);
    ui->stackedWidget->setCurrentWidget(this->pSliceWidget);
    this->showMainToolbar();
}

void MainWindow::showMainToolbar(void)
{
    qDebug() << Q_FUNC_INFO;

    ui->tlbMain->setVisible(true);
    this->actChangeWindow->setVisible(true);
}

void MainWindow::hideMainToolbar(void)
{
    qDebug() << Q_FUNC_INFO;

    ui->tlbMain->setVisible(false);
    this->actChangeWindow->setVisible(false);
}

void MainWindow::displayWaitCursor(void)
{
    this->setCursor(Qt::WaitCursor);
}

void MainWindow::displayArrowCursor(void)
{
    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::setupToolbar(void)
{
    /* Create a combo box in the toolbar with the window/width levels */
    QComboBox *pCBWindows = new QComboBox(ui->tlbMain);
    Q_ASSERT(pCBWindows);

    /* Add the same items as those found in View->Window menu*/
    QStringList cbItems;
    cbItems << "Abdomen" << "Bone" << "Head" << "Lung" << "Mediastinum" << "Soft tissue";
    pCBWindows->addItems(cbItems);

    connect(pCBWindows, SIGNAL(currentTextChanged(QString)),
            this, SLOT(changeWindow(QString)));

    this->actChangeWindow = new QWidgetAction(NULL);
    Q_ASSERT(this->actChangeWindow);
    this->actChangeWindow->setDefaultWidget(pCBWindows);

    /* By default, don't show it until we actually load an image series */
    this->actChangeWindow->setVisible(false);  // this must come *AFTER* setDefaultWidget()
    ui->tlbMain->addAction(actChangeWindow);

    /* By default, the whole main toolbar isn't visible upon program start */
    ui->tlbMain->setVisible(false);
}
