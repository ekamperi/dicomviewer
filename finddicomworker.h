#ifndef FINDDICOMFILESWORKER_H
#define FINDDICOMFILESWORKER_H

#include <QDebug>
#include <QMutex>
#include <QThread>

#include "patientexplorer.h"

class FindDicomThread : public QThread
{
    Q_OBJECT

public:
    FindDicomThread(PatientExplorer *pPatientExplorer, QObject *parent);
    void run();
    void abortOperation();

private:
    QMutex mutex;
    bool abort;

    PatientExplorer *patientExplorer;
};

#endif // FINDDICOMFILESWORKER_H
