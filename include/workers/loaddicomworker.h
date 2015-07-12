#ifndef LOADDICOMWORKER_H
#define LOADDICOMWORKER_H

#include "slice.h"

#include <QMutex>
#include <QStringList>
#include <QThread>

class LoadDicomThread : public QThread
{
    Q_OBJECT

public:
    LoadDicomThread(const QStringList &fileNames, QVector<Slice *> *slices, QObject *parent);
    void run();
    void abortOperation();

signals:
    void reportProgress(unsigned int cnt);

private:
    QStringList fileNames;
    QMutex mutex;
    bool abort;

    QVector<Slice *> *pResults;
};

#endif // LOADDICOMWORKER_H
