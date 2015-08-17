#include "include/widgets/patientexplorerwidget.h"
#include "ui_patientexplorerwidget.h"

#include "include/workers/finddicomworker.h"
#include "patientexplorer.h"

#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>

PatientExplorerWidget::PatientExplorerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PatientExplorerWidget)
{
    ui->setupUi(this);

    /* Setup column width in the tree widget */
    ui->treePatients->setColumnWidth(0, 350);
    ui->treePatients->setColumnWidth(1,  70);
    ui->treePatients->setColumnWidth(2,  30);
    ui->treePatients->setColumnWidth(3, 150);
    ui->treePatients->setColumnWidth(4, 150);

    /* Create a patient explorer object that will do the actuall scanning */
    this->pPatientExplorer = new PatientExplorer();
    Q_ASSERT(this->pPatientExplorer);
    connect(this->pPatientExplorer, SIGNAL(reportProgress(unsigned int, unsigned int)),
            this, SLOT(readProgress(unsigned int, unsigned int)));

    /* We want to be notified when item selection changes */
    connect(ui->treePatients, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_itemSelectionChanged()));

    /* We want to be notified when user clicks on a patient image series */
    connect(ui->treePatients, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(on_itemDoubleClicked(QTreeWidgetItem*,int)));
}

PatientExplorerWidget::~PatientExplorerWidget()
{
    delete ui;
    Q_ASSERT(this->pPatientExplorer);
    delete this->pPatientExplorer;
}

void PatientExplorerWidget::keyPressEvent(QKeyEvent *pEvent)
{
    qDebug() << Q_FUNC_INFO;

    if (pEvent->key() == Qt::Key_Escape) {
        this->close();
        return;
    } else if (pEvent->key() == Qt::Key_Space) {
        /* XXX: This needs to be fixed (use an event filter?) */
        QList<QTreeWidgetItem *> selectedItems = ui->treePatients->selectedItems();
        for (int i = 0; i < selectedItems.size(); i++) {
            QTreeWidgetItem *pItem = selectedItems.at(i);
            bool expand = pItem->isExpanded();
            selectedItems.at(i)->setExpanded(!expand);
            QWidget::keyPressEvent(pEvent);
        }
    } else if (pEvent->key() == Qt::Key_Return) {
        this->on_itemDoubleClicked(ui->treePatients->selectedItems().at(0), 0);
    } else {
        QWidget::keyPressEvent(pEvent);
    }
}

void PatientExplorerWidget::on_btnBrowse_clicked()
{
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

    /* Update the edit text with the path */
    ui->editPath->setText(dir);

    /* Should we do a recursive scan ? */
    bool isRecursive = ui->ckbSearchRecursively->isChecked();

    /* Do the actual scan */
    this->doScan(dir, isRecursive);
}

void PatientExplorerWidget::doScan(QString dir, bool isRecursive)
{
    /* Show a progress dialog */
    this->progressDialog = new QProgressDialog(
                "This may take a while...", "Abort operation", 0, 0, this);
    Q_ASSERT(this->progressDialog);

    /* Make the width of the dialog fixed, otherwise it will resize like crazy
     * while the scanned files increase (in order to accomodate the text).
    */
    this->progressDialog->setFixedWidth(500);

    this->progressDialog->setWindowTitle("Scanning for DICOM files");
    this->progressDialog->setWindowModality(Qt::WindowModal);
    connect(this->progressDialog, SIGNAL(canceled()),
            this, SLOT(progressDialogCanceled()));
    this->progressDialog->show();

    /*
     * The actual scanning shall go to its own thread, so that it doesn't
     * block the main GUI thread. The PatientExplorer object is responsible
     * for that.
    */
    this->pPatientExplorer->setPath(dir);
    this->pPatientExplorer->setRecursiveSearch(isRecursive);

    FindDicomThread *findDicomThread = new FindDicomThread(this->pPatientExplorer, this);
    Q_ASSERT(findDicomThread);

    connect(findDicomThread, SIGNAL(finished()),
            findDicomThread, SLOT(deleteLater()));
    connect(findDicomThread, SIGNAL(finished()),
            this, SLOT(filesScanned()));

    /* Fire off the worker thread! (it doesn't block) */
    findDicomThread->start();
}

QTreeWidgetItem *PatientExplorerWidget::addTreeRoot(const void *obj, int type)
{
    qDebug() << Q_FUNC_INFO;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treePatients, type);
    Q_ASSERT(treeItem);

    Patient patient = *(const Patient *)obj;
    treeItem->setText(0, patient.getName());
    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(patient));

    return treeItem;
}

QTreeWidgetItem *PatientExplorerWidget::addTreeChild(QTreeWidgetItem *parent, const void *obj, int type)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(obj);
    Q_ASSERT(type == TypeStudy || type == TypeSeries);

    /* Wee need to pass `type' to constructor, otherwise we won't be able to
     * deduce what item type was clicked in `on_itemSelectionChanged()`
     */
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(type);
    Q_ASSERT(treeItem);

    if (type == TypeStudy) {
        Study study = *(const Study *)obj;
        treeItem->setText(0, study.getDesc());
        treeItem->setText(1, "");
        treeItem->setText(2, "");
        treeItem->setText(3, study.getDate() + " " + study.getTime());
        treeItem->setText(4, study.getUID());
        treeItem->setData(0, Qt::UserRole, QVariant::fromValue(study));
    } else if (type == TypeSeries) {
        Series series = *(const Series *)obj;
        treeItem->setText(0, series.getDesc());
        treeItem->setText(1, series.getModality());
        treeItem->setText(2, QString::number(series.getNumOfImages()));
        treeItem->setText(3, series.getDate() + " " + series.getTime());
        treeItem->setText(4, series.getUID());
        treeItem->setData(0, Qt::UserRole, QVariant::fromValue(series));
    } else {
        qDebug() << "Unsupported item type (shouldn't happen)!";
    }

    /* Add child to tree */
    parent->addChild(treeItem);

    return treeItem;
}

void PatientExplorerWidget::filesScanned()
{
    qDebug() << Q_FUNC_INFO;

    /* Close the progress dialog */
    this->progressDialog->deleteLater();

    /* Remove old items */
    ui->treePatients->clear();

    /* Populate the tree widget */
    Q_ASSERT(this->pPatientExplorer);
    const PatientExplorer &pe = *this->pPatientExplorer;

    QList<Patient> patients = this->pPatientExplorer->getPatients();
    for (int i = 0; i < patients.size(); i++) {
        QTreeWidgetItem *parent = this->addTreeRoot(&patients.at(i), TypePatient);
        Q_ASSERT(parent);

        /* For every patient, add the related studies */
        QList<Study> studies = pe.getStudies(patients.at(i));
        for (int j = 0; j < studies.size(); j++) {
            QTreeWidgetItem *parent2 = this->addTreeChild(parent, &studies.at(j), TypeStudy);
            Q_ASSERT(parent2);

            /* For every study, add the related series */
            QList<Series> series = pe.getSeries(patients.at(i), studies.at(j));
            for (int k = 0; k < series.size(); k++) {
                this->addTreeChild(parent2, &series.at(k), TypeSeries);
            }
        }
    }

    /* If there's only one patient, expand all items */
    int nPatients = pe.getPatients().size();
    if (nPatients == 1) {
        ui->treePatients->expandAll();
    }

    /* Update the status bar */
    QString newText;
    if (nPatients == 1) {
        newText = "1 patient was found.";
    } else {
        newText = QString::number(nPatients) + " patients were found.";
    }
    ui->lblStatusBar->setText(newText);
}

void PatientExplorerWidget::readProgress(unsigned int scannedFiles,
                                         unsigned int parsedFiles)
{
    qDebug() << Q_FUNC_INFO;

    if (!this->progressDialog->wasCanceled()) {
        QString newString =
                QString("This may take a while... (%1 found out of %2 scanned)").arg(
                    parsedFiles).arg(scannedFiles);
        this->progressDialog->setLabelText(newString);
    }
}

void PatientExplorerWidget::progressDialogCanceled()
{
    qDebug() << Q_FUNC_INFO;
    this->pPatientExplorer->abortScanning();
}

void PatientExplorerWidget::on_itemSelectionChanged(void)
{
    qDebug() << Q_FUNC_INFO;

    QList<QTreeWidgetItem *> selItems = ui->treePatients->selectedItems();
    for (int i = 0; i < selItems.size(); i++) {
        QTreeWidgetItem *pItem = selItems[i];
        Patient patient;
        Study study;
        Series series;
        switch(pItem->type()) {
        case TypePatient:
            patient = pItem->data(0, Qt::UserRole).value<Patient>();
            qDebug() << "Patient:" << patient.getName();
            break;
        case TypeStudy:
            study = pItem->data(0, Qt::UserRole).value<Study>();
            patient = pItem->parent()->data(0, Qt::UserRole).value<Patient>();
            break;
        case TypeSeries:
            series = pItem->data(0, Qt::UserRole).value<Series>();
            study = pItem->parent()->data(0, Qt::UserRole).value<Study>();
            patient = pItem->parent()->parent()->data(0, Qt::UserRole).value<Patient>();
            break;
        default:
           qDebug() << "User clicked on an unsupported item type (shouldn't happen)!";
        }
    }
}

void PatientExplorerWidget::loadDroppedPatient(void)
{
    qDebug() << Q_FUNC_INFO;
    QTreeWidgetItem *pDraggedItem = ui->treePatients->getDraggedItem();
    //Q_ASSERT(pDraggedItem);
    if (!pDraggedItem)
        return;
    this->on_itemDoubleClicked(pDraggedItem, 0);    // XXX
}

void PatientExplorerWidget::on_itemDoubleClicked(QTreeWidgetItem *pTreeItem, int col)
{
    Q_ASSERT(pTreeItem);
    qDebug() << Q_FUNC_INFO << pTreeItem->text(0);

    /* We only switch to work space when user clicks on an image series */
    Patient patient;
    Study study;
    Series series;
    if (pTreeItem && pTreeItem->type() == TypeSeries) {
        series = pTreeItem->data(0, Qt::UserRole).value<Series>();
        study = pTreeItem->parent()->data(0, Qt::UserRole).value<Study>();
        patient = pTreeItem->parent()->parent()->data(0, Qt::UserRole).value<Patient>();

        QList<QString> fileList = this->pPatientExplorer->getMap()[patient][study][series];
        QStringList sortedFileList = QStringList(fileList);
        sortedFileList.sort(Qt::CaseInsensitive);    // XXX
        emit this->loadSeries(sortedFileList);
    }
}

void PatientExplorerWidget::on_editPath_returnPressed()
{
    qDebug() << Q_FUNC_INFO;
    this->on_btnScan_clicked();
}

/* We modify the UI of the patient explorer widget based on whether it is
 * floating or not. If it ain't floating, we turn it into a sort of case
 * explorer widget of currently available patients.
 */
void PatientExplorerWidget::updateUI(bool isFloating)
{
    qDebug() << Q_FUNC_INFO;

    if (isFloating) {
        ui->btnBrowse->show();
        ui->btnScan->show();
        ui->ckbSearchRecursively->show();
        ui->editPath->show();
        ui->lblStatusBar->show();
    } else {
        ui->btnBrowse->hide();
        ui->btnScan->hide();
        ui->ckbSearchRecursively->hide();
        ui->editPath->hide();
        ui->lblStatusBar->hide();
    }
}

void PatientExplorerWidget::on_btnScan_clicked()
{
    qDebug() << Q_FUNC_INFO;

    QString path = ui->editPath->text();
    if (path.size() > 0) {
        bool isRecursive = ui->ckbSearchRecursively->isChecked();
        this->doScan(path, isRecursive);
    }
}
