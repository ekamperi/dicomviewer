#include "patientexplorer.h"

#include <QDirIterator>
#include <QDebug>

PatientExplorer::PatientExplorer(QString path)
{
    qDebug() << Q_FUNC_INFO;

    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        this->getPatientName(it.next());
    }
}

void PatientExplorer::getPatientName(QString path)
{
    qDebug() << Q_FUNC_INFO << "path =" << path;

    /* Open dicom file */
    OFCondition status = this->dcmFile.loadFile(path.toStdString().c_str());
    if (!status.good()) {
        qDebug() << "1" << status.text();
        return;
    }

    /* Get pointer to dataset */
    DcmDataset *pDcmDataset = dcmFile.getDataset();
    if (!pDcmDataset) {
        qDebug() << "pDcmDataset is null";
        return;
    }

    /* Get patient ID */
    OFString result;
    status = pDcmDataset->findAndGetOFString(DCM_PatientID, result);
    QString patientID;
    if (status.good()) {
        patientID = QString(result.c_str());
    } else {
        patientID = QString("N/A");
    }
    qDebug() << "Found" << patientID;

    /* Get study */
}
