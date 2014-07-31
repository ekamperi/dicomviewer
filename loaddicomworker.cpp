#include "dicomfile.h"
#include "loaddicomworker.h"
#include "myglwidget.h"

#include <QDebug>

LoadDicomThread::LoadDicomThread(const QStringList &fileNames, QObject *parent)
    : QThread(parent), fileNames(fileNames)
{
    qDebug() << Q_FUNC_INFO;
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
