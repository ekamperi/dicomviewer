#include "finddicomworker.h"
#include "patientexplorer.h"

FindDicomThread::FindDicomThread(
        PatientExplorer *pPatientExplorer, QObject *parent) :
    QThread(parent)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(pPatientExplorer);
    this->patientExplorer = pPatientExplorer;
}

void FindDicomThread::run()
{
    this->patientExplorer->doScan();
}

void FindDicomThread::abortOperation()
{
    qDebug() << Q_FUNC_INFO;

    this->mutex.lock();
    this->abort = true;
    this->mutex.unlock();
};
