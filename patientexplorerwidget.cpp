#include "patientexplorerwidget.h"
#include "ui_patientexplorerwidget.h"

#include "patientexplorer.h"

#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>

PatientExplorerWidget::PatientExplorerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PatientExplorerWidget)
{
    ui->setupUi(this);
}

PatientExplorerWidget::~PatientExplorerWidget()
{
    delete ui;
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
        for (unsigned int i = 0; i < selectedItems.size(); i++) {
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

    /* XXX: This shall go to its own thread */
    PatientExplorer pe(dir);

    /* Remove old items */
    ui->treePatients->clear();

    /* Populate the tree widget */
    QList<QString> patients = pe.getPatients();
    for (int i = 0; i < patients.size(); i++) {
        QTreeWidgetItem *parent = this->addTreeRoot(patients.at(i));
        Q_ASSERT(parent);

        /* For every patient, add the related studies */
        QList<QString> studies = pe.getStudies(patients.at(i));
        for (int j = 0; j < studies.size(); j++) {
            QTreeWidgetItem *parent2 = this->addTreeChild(parent, studies.at(j));
            Q_ASSERT(parent2);

            /* For every study, add the related series */
            QList<QString> series = pe.getSeries(patients.at(i), studies.at(j));
            for (unsigned k = 0; k < series.size(); k++) {
                this->addTreeChild(parent2, series.at(k));
            }
        }
    }

    /* Update the status bar */
    ui->lblStatusBar->setText(
                QString::number(pe.getPatients().size()) +
                " patient(s) were found.");
}

QTreeWidgetItem *PatientExplorerWidget::addTreeRoot(QString name)
{
    qDebug() << Q_FUNC_INFO;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treePatients);
    Q_ASSERT(treeItem);
    treeItem->setText(0, name);

    return treeItem;
}

QTreeWidgetItem *PatientExplorerWidget::addTreeChild(QTreeWidgetItem *parent,
                  QString name)
{
    qDebug() << Q_FUNC_INFO;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    Q_ASSERT(treeItem);
    treeItem->setText(0, name);
    parent->addChild(treeItem);

    return treeItem;
}
