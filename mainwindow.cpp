#include "mainwindow.h"
#include "ui_mainwindow.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

#include <iostream>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
}

void MainWindow::on_actionOpenDICOM_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this);

    DcmFileFormat dcmFile;
    OFCondition status = dcmFile.loadFile(filename.toStdString().c_str());
    if (status.good()) {
        OFString patientName;
        status = dcmFile.getDataset()->findAndGetOFString(
                    DCM_PatientName, patientName);
        if (status.good()) {
            qDebug() << "Patient name:" << patientName.c_str();
        } else {
            qWarning() << status.text();
        }
    } else {
        qWarning() << status.text();
    }

    /* Iterate over all objects */
    DcmDataset *pDcmDataset = dcmFile.getDataset();
    DcmStack stack;
    DcmObject *obj = NULL;
    while (pDcmDataset->nextObject(stack, /* Depth-first */ OFTrue).good()) {
        obj = stack.top();
        obj->print(std::cout);
    }
}
