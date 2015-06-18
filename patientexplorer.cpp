#include "patientexplorer.h"
#include "dicomhelper.h"

#include <QDirIterator>
#include <QDebug>

PatientExplorer::PatientExplorer(QObject *parent) : QObject(parent)
{
    qDebug() << Q_FUNC_INFO;
    abort = false;
}

PatientExplorer::PatientExplorer(QString path, QObject *parent) : QObject(parent)
{
    qDebug() << Q_FUNC_INFO;
    this->path = path;
    abort = false;
}

void PatientExplorer::doScan(void)
{
    qDebug() << Q_FUNC_INFO;

    /* Make sure path is not empty */
    Q_ASSERT(!this->path.isEmpty());

    /* Remove any old items in the resultant map */
    this->myMap.clear();

    /* Iterate RECURSIVELY over all files in a directory */
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    unsigned int scannedFiles = 0;
    while (it.hasNext() && !this->abort) {
        this->extract(it.next());
        ++scannedFiles;
        /* Every now and then emit a signal to update the progress bar, if any */
        if (scannedFiles > 500) {
            emit this->reportProgress(scannedFiles);
        }
    }

    emit this->reportProgress(scannedFiles);

    /* Clear it or it will become sticky and affect subsequent scans */
    this->abort = false;
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

QList<QString> PatientExplorer::getPatients(void) const
{
    qDebug() << Q_FUNC_INFO;

    QList<QString> patients = this->myMap.keys();
    return patients;
}

QList<QString> PatientExplorer::getStudies(const QString &patientName) const
{
    qDebug() << Q_FUNC_INFO;

    QList<QString> studies;
    QMap<QString, StudyMap>::const_iterator it = this->myMap.find(patientName);
    while (it != this->myMap.end() && it.key() == patientName) {
        QList<QString> keys = it.value().keys();
        for (int j = 0; j < keys.size(); j++) {
            studies.append(keys.at(j));
        }
        ++it;
    }
    return studies;
}

QList<QString> PatientExplorer::getSeries(const QString &patientName, const QString &studyID) const
{
    qDebug() << Q_FUNC_INFO;

    SeriesMap mySeriesMap = this->myMap[patientName][studyID];

    return mySeriesMap.keys();;
}

void PatientExplorer::abortScanning(void)
{
    qDebug() << Q_FUNC_INFO;
    this->abort = true;
}
