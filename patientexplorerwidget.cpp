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

void PatientExplorerWidget::keyPressEvent(QKeyEvent *pEvent)
{
    if (pEvent->key() == Qt::Key_Escape) {
        this->close();
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
    for (unsigned int i = 0; i < patients.size(); i++) {
        QTreeWidgetItem *parent = this->addTreeRoot(patients.at(i));

        /* For every patient, add the related studies */
        QList<QString> studies = pe.getStudies(patients.at(i));
        for (unsigned int j = 0; j < studies.size(); j++) {
            this->addTreeChild(parent, studies.at(j));
        }
    }
}

QTreeWidgetItem *PatientExplorerWidget::addTreeRoot(QString name)
{
    qDebug() << Q_FUNC_INFO;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treePatients);
    Q_ASSERT(treeItem);
    treeItem->setText(0, name);

    return treeItem;
}

void PatientExplorerWidget::addTreeChild(QTreeWidgetItem *parent,
                  QString name)
{
    qDebug() << Q_FUNC_INFO;

    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    Q_ASSERT(treeItem);
    treeItem->setText(0, name);
    parent->addChild(treeItem);
}
