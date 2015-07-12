#include "dicomfile.h"
#include "include/workers/loaddicomworker.h"
#include "include/widgets/myglwidget.h"

#include <QDebug>

LoadDicomThread::LoadDicomThread(
        const QStringList &fileNames,
        QVector<Slice *> *results,
        QObject *parent)
    : QThread(parent)
{
    this->pResults = results;
    this->fileNames = fileNames;
}

void LoadDicomThread::run()
{
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

        /* Save result */
        this->pResults->push_back(
                    new Slice(fileNames[i], i));

        /* Notify the progress bar on our progress */
        emit this->reportProgress(i);
    }
}

void LoadDicomThread::abortOperation()
{
    this->mutex.lock();
    this->abort = true;
    this->mutex.unlock();
};

