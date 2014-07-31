#ifndef LOADDICOMWORKER_H
#define LOADDICOMWORKER_H

#include <QMutex>
#include <QStringList>
#include <QThread>

class LoadDicomThread : public QThread
{
    Q_OBJECT

public:
    LoadDicomThread(const QStringList &fileNames, QObject *parent);
    void run();
    void abortOperation();
signals:
    void reportProgress(unsigned int cnt);

private:
    QStringList fileNames;
    QMutex mutex;
    bool abort;
};

#endif // LOADDICOMWORKER_H