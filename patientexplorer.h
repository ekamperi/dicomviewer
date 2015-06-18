#ifndef PATIENTEXPLORER_H
#define PATIENTEXPLORER_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QObject>

#include "study.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

/* <Patient Name, <Study, <Series, File>>> */
typedef QMap<QString, QMap<QString, QString> >  StudyMap;
typedef QMap<QString, QString> SeriesMap;

class PatientExplorer : public QObject
{
    Q_OBJECT

public:
    PatientExplorer(QObject *parent = NULL);
    PatientExplorer(QString path, QObject *parent = NULL);
    void setPath(const QString &path) { this->path = path; }
    void doScan(void);

    QList<QString> getPatients(void) const;
    QList<QString> getStudies(const QString &patientName) const;
    QList<QString> getSeries(const QString &patientName, const QString &studyID) const;

    void abortScanning(void);

signals:
    void reportProgress(unsigned int scannedFiles);

private:
    void extract(QString path);

    QString path;
    bool abort;

    /* <Patient, <Study, <Series, Path>>> */
    QMap<QString, QMap<QString, QMap<QString, QString> > > myMap;
};

#endif // PATIENTEXPLORER_H
