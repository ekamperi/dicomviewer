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

    this->pResults = results;
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

        /* Save result */
        this->pResults->push_back(
                    new Slice(fileNames[i], i));

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

