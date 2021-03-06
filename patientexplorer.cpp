#include "patientexplorer.h"
#include "include/helpers/dicomhelper.h"

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

    /* Iterate over all files in a directory. To recurse or not ? */
    QDirIterator::IteratorFlags iterFlags;
    iterFlags = this->recursiveSearch ?
                QDirIterator::Subdirectories :
                QDirIterator::NoIteratorFlags;
    QDirIterator it(this->path, QDir::Files | QDir::NoDotAndDotDot, iterFlags);

    unsigned int scannedFiles = 0;
    unsigned int parsedFiles = 0;
    while (it.hasNext() && !this->abort) {
        ++scannedFiles;
        bool wasParsed = this->extract(it.next());
        if (wasParsed) {
            ++parsedFiles;
        }
        /* Every now and then emit a signal to update the progress bar, if any */
        if (scannedFiles % 500 == 0) {
            emit this->reportProgress(scannedFiles, parsedFiles);
        }
    }

    emit this->reportProgress(scannedFiles, parsedFiles);

    /* Clear it or it will become sticky and affect subsequent scans */
    this->abort = false;
}

bool PatientExplorer::extract(QString path)
{
    qDebug() << Q_FUNC_INFO << "path =" << path;

    /* Open dicom file */
    DcmFileFormat dcmFile;
    OFCondition status = dcmFile.loadFile(path.toStdString().c_str());
    if (!status.good()) {
        /* Most likely, this is *not* a dicom file, so just skip it */
        return false;
    }

    /* Get pointer to dataset */
    DcmDataset *pDcmDataset = dcmFile.getDataset();
    if (!pDcmDataset) {
        qDebug() << "pDcmDataset is null";
        return false;
    }

    /* Patient > Study > Series */
    DcmTagKey keys[] = {
        DCM_PatientName,
        DCM_StudyInstanceUID, DCM_StudyDescription, DCM_StudyDate, DCM_StudyTime,
        DCM_SeriesInstanceUID, DCM_SeriesDescription, DCM_SeriesDate, DCM_SeriesTime,
        DCM_Modality };

    /* Calculate size of array */
    size_t len = sizeof(keys) / sizeof(keys[0]);

    /* Retrieve values for keys and populate the map */
    QString *res = new QString[len];
    for (unsigned int i = 0; i < len; i++) {
        res[i] = DicomHelper::getDcmTagKeyAsQString(pDcmDataset, keys[i]);
    }

    /* Add item to map */
    Patient patient(res[0]);
    Study   study(res[1], res[2], res[3], res[4]);
    Series series(res[5], res[6], res[7], res[8], res[9], 0);
    this->myMap[patient][study][series].push_back(path);

    delete[] res;

    /* True means that this was a dicom file and it was parsed succesfully */
    return true;
}

QList<Patient> PatientExplorer::getPatients(void) const
{
    qDebug() << Q_FUNC_INFO;

    QList<Patient> patients = this->myMap.keys();
    return patients;
}

QList<Study> PatientExplorer::getStudies(const Patient &patient) const
{
    qDebug() << Q_FUNC_INFO;

    QList<Study> studies;
    QMap<Patient, StudyMap>::const_iterator it = this->myMap.find(patient);
    while (it != this->myMap.end() && it.key().getName() == patient.getName()) {
        QList<Study> keys = it.value().keys();
        for (int j = 0; j < keys.size(); j++) {
            studies.append(keys.at(j));
        }
        ++it;
    }
    return studies;
}

QList<Series> PatientExplorer::getSeries(const Patient &patient, const Study &study) const
{
    qDebug() << Q_FUNC_INFO;

    SeriesMap mySeriesMap = this->myMap[patient][study];
    return mySeriesMap.keys();  // if series is a multi map, use uniqueKeys()
}

int PatientExplorer::getNumberOfImages(const Patient &patient, const Study &study, const Series &series) const
{
    qDebug() << Q_FUNC_INFO;

    return this->myMap[patient][study][series].size();
}

void PatientExplorer::abortScanning(void)
{
    qDebug() << Q_FUNC_INFO;
    this->abort = true;
}
