#include "patientexplorer.h"
#include "dicomhelper.h"

#include <QDirIterator>
#include <QDebug>

PatientExplorer::PatientExplorer(QString path)
{
    qDebug() << Q_FUNC_INFO;

    /* Iterate RECURSIVELY over all files in a directory */
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        this->extract(it.next());
    }
}

void PatientExplorer::extract(QString path)
{
    qDebug() << Q_FUNC_INFO << "path =" << path;

    /* Open dicom file */
    DcmFileFormat dcmFile;
    OFCondition status = dcmFile.loadFile(path.toStdString().c_str());
    if (!status.good()) {
        qDebug() << "Unable to load DICOM file:" << status.text();
        return;
    }

    /* Get pointer to dataset */
    DcmDataset *pDcmDataset = dcmFile.getDataset();
    if (!pDcmDataset) {
        qDebug() << "pDcmDataset is null";
        return;
    }

    /* Patient > Study > Series */
    DcmTagKey keys[] = { DCM_PatientName, DCM_StudyInstanceUID, DCM_SeriesInstanceUID };

    /* Calculate size of array */
    size_t len = sizeof(keys) / sizeof(keys[0]);

    /* Retrieve values for keys and populate the map */
    QString *res = new QString[len];
    for (unsigned int i = 0; i < len; i++) {
        res[i] = DicomHelper::getDcmTagKeyAsQString(pDcmDataset, keys[i]);
   }

    /* Add item to map */
    QString patientName = res[0];
    QString studyID     = res[1];
    QString seriesID    = res[2];
    this->myMap[patientName][studyID][seriesID] = path;

    delete[] res;
}

QList<QString> PatientExplorer::getPatients(void)
{
    qDebug() << Q_FUNC_INFO;

    QList<QString> patients = this->myMap.keys();
    return patients;
}

QList<QString> PatientExplorer::getStudies(const QString &patientName)
{
    qDebug() << Q_FUNC_INFO;

    QList<QString> studies;
    QMap<QString, StudyMap>::const_iterator it = this->myMap.find(patientName);
    while (it != this->myMap.end() && it.key() == patientName) {
        studies.append(it.value().keys().at(0));
        ++it;
    }
    return studies;
}

QList<QString> PatientExplorer::getSeries(const QString &patientName, const QString &studyID)
{
    qDebug() << Q_FUNC_INFO;

    QList<QString> series;
    return series;
}
