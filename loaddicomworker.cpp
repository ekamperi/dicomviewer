#include "dicomfile.h"
#include "loaddicomworker.h"
#include "myglwidget.h"

#include <QDebug>

LoadDicomThread::LoadDicomThread(
        const QStringList &fileNames,
        std::vector<Slice *> *results,
        QObject *parent)
    : QThread(parent)
{
    qDebug() << Q_FUNC_INFO;

    this->results = results;
    this->fileNames = fileNames;
}

void LoadDicomThread::run()
{
    qDebug() << Q_FUNC_INFO;

    this->mutex.lock();
    this->abort = false;
    this->mutex.unlock();

    for (int i = 0; i < fileNames.size(); i++) {
        this->mutex.lock();
        bool abort2 = this->abort;
        this->mutex.unlock();

        if (abort2) {
            qDebug() << "LoadDicomThread WAS CANCELED!";
            break;
            /* NEVER REACHED */
        }

        /* Create a DicomFile object to dissect the input file */
        DicomFile *pDicomFile = new DicomFile();
        pDicomFile->loadDicomFile(fileNames[i]);

        /* Extract the raw pixel data from the DICOM file */
        unsigned char *pRawPixelData =
                pDicomFile->getCompressedData();
        unsigned int width = pDicomFile->getWidth();
        unsigned int height = pDicomFile->getHeight();

        /* Also extract examination details */
        ExamDetails examDetails = pDicomFile->getExamDetails();

        /* Save result */
        this->results->push_back(
                    new Slice(
                        pRawPixelData, width, height,
                        examDetails));

        emit this->reportProgress(i);
    }
}

void LoadDicomThread::abortOperation()
{
    qDebug() << Q_FUNC_INFO;

    this->mutex.lock();
    this->abort = true;
    this->mutex.unlock();
};

