#ifndef PATIENTEXPLORER_H
#define PATIENTEXPLORER_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QObject>

#include "patient.h"
#include "series.h"
#include "study.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

/* <Patient Name, <Study, <Series, File>>> */
typedef QMap<Study, QMap<Series, QString> >  StudyMap;
typedef QMap<Series, QString> SeriesMap;

class PatientExplorer : public QObject
{
    Q_OBJECT

public:
    PatientExplorer(QObject *parent = NULL);
    PatientExplorer(QString path, QObject *parent = NULL);
    void setPath(const QString &path) { this->path = path; }
    void doScan(void);

    QList<Patient> getPatients(void) const;
    QList<Study> getStudies(const Patient &patient) const;
    QList<Series> getSeries(const Patient &patient, const Study &study) const;
    int getNumberOfImages(const Patient &patient, const Study &study, const Series &series) const;

    void abortScanning(void);

signals:
    void reportProgress(unsigned int scannedFiles);

private:
    void extract(QString path);

    QString path;
    bool abort;

    /* <Patient, <Study, <Series, Path>>> */
    QMap<Patient, QMap<Study, QMap<Series, QString> > > myMap;
};

#endif // PATIENTEXPLORER_H
