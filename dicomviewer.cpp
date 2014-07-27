#include <iostream>
#include <QDebug>

#include "dicomviewer.h"
#include "ui_dicomviewer.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

DicomViewer::DicomViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DicomViewer)
{
    ui->setupUi(this);
}

DicomViewer::~DicomViewer()
{
    delete ui;
}

void DicomViewer::loadDicomFile(QString filename)
{
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
