#include "patientexplorerwidget.h"
#include "ui_patientexplorerwidget.h"

#include "finddicomworker.h"
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
    ui->treePatients->setColumnWidth(0, 400);
    ui->treePatients->setColumnWidth(1, 100);
    ui->treePatients->setColumnWidth(2, 150);

    /* Create a patient explorer object that will do the actuall scanning */
    this->pPatientExplorer = new PatientExplorer();
    Q_ASSERT(this->pPatientExplorer);
    connect(this->pPatientExplorer, SIGNAL(reportProgress(uint)),
            this, SLOT(readProgress(uint)));
}

PatientExplorerWidget::~PatientExplorerWidget()
{
    delete ui;
    Q_ASSERT(this->pPatientExplorer);
    delete this->pPatientExplorer;
}

void PatientExplorerWidget::on_itemSelectionChanged(void)
{
    qDebug() << Q_FUNC_INFO;
}

void PatientExplorerWidget::keyPressEvent(QKeyEvent *pEvent)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "YES!";

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

    ui->editPath->setText(dir);

    /*
     * The actual scanning shall go to its own thread, so that it doesn't
       block the main GUI thread.
    */
    this->pPatientExplorer->setPath(dir);

    FindDicomThread *findDicomThread = new FindDicomThread(this->pPatientExplorer, this);
    Q_ASSERT(findDicomThread);

    connect(findDicomThread, SIGNAL(finished()),
            findDicomThread, SLOT(deleteLater()));
    connect(findDicomThread, SIGNAL(finished()),
            this, SLOT(filesScanned()));

    /* Show a progress dialog */
    this->progressDialog = new QProgressDialog(
                "This may take a while...", "Abort operation", 0, 0, this);
    Q_ASSERT(this->progressDialog);
    this->progressDialog->setWindowTitle("Scanning for DICOM files");
    this->progressDialog->setWindowModality(Qt::WindowModal);
    connect(this->progressDialog, SIGNAL(canceled()),
            this, SLOT(progressDialogCanceled()));
    this->progressDialog->show();

    /* Fire off the worker thread! (it doesn't block) */
    findDicomThread->start();
}

QTreeWidgetItem *PatientExplorerWidget::addTreeRoot(QString name, QString desc)
{
    qDebug() << Q_FUNC_INFO;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treePatients);
    Q_ASSERT(treeItem);
    treeItem->setText(0, name);
    treeItem->setText(1, desc);

    return treeItem;
}

QTreeWidgetItem *PatientExplorerWidget::addTreeChild(QTreeWidgetItem *parent,
                  const Study &study)
{
    qDebug() << Q_FUNC_INFO;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    Q_ASSERT(treeItem);
    treeItem->setText(0, study.getDesc());
    treeItem->setText(1, study.getDate());
    treeItem->setText(2, study.getUID());

    parent->addChild(treeItem);

    return treeItem;
}

QTreeWidgetItem *PatientExplorerWidget::addTreeSeries(QTreeWidgetItem *parent,
                  const Series &series)
{
    qDebug() << Q_FUNC_INFO;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    Q_ASSERT(treeItem);
    treeItem->setText(0, series.getDesc());
    treeItem->setText(1, series.getDate());
    treeItem->setText(2, series.getUID());

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

    QList<QString> patients = this->pPatientExplorer->getPatients();
    for (int i = 0; i < patients.size(); i++) {
        QTreeWidgetItem *parent = this->addTreeRoot(patients.at(i), "");
        Q_ASSERT(parent);

        /* For every patient, add the related studies */
        QList<Study> studies = pe.getStudies(patients.at(i));
        for (int j = 0; j < studies.size(); j++) {
            QTreeWidgetItem *parent2 = this->addTreeChild(parent, studies.at(j));
            Q_ASSERT(parent2);

            /* For every study, add the related series */
            QList<Series> series = pe.getSeries(patients.at(i), studies.at(j));
            for (int k = 0; k < series.size(); k++) {
                this->addTreeSeries(parent2, series.at(k));
            }
        }
    }

    /* Update the status bar */
    ui->lblStatusBar->setText(
                QString::number(pe.getPatients().size()) +
                " patient(s) were found.");
}

void PatientExplorerWidget::readProgress(unsigned int scannedFiles)
{
    qDebug() << Q_FUNC_INFO;

    if (!this->progressDialog->wasCanceled()) {
        QString newString = QString("This may take a while... (%1 found)").arg(scannedFiles);
        this->progressDialog->setLabelText(newString);
    }
}

void PatientExplorerWidget::progressDialogCanceled()
{
    qDebug() << Q_FUNC_INFO;
    this->pPatientExplorer->abortScanning();
}
