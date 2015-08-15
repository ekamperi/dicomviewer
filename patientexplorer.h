#ifndef PATIENTEXPLORER_H
#define PATIENTEXPLORER_H

#include <QMap>
#include <QTreeWidgetItem>
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
typedef QMap<Patient, QMap<Study, QMap<Series, QList<QString> > > > PatientMap;
typedef QMap<Study, QMap<Series, QList<QString> > >  StudyMap;
typedef QMap<Series, QList<QString> > SeriesMap;

class PatientExplorer : public QObject
{
    Q_OBJECT

public:
    PatientExplorer(QObject *parent = NULL);
    PatientExplorer(QString path, QObject *parent = NULL);
    void setPath(const QString &path) { this->path = path; }
    void setRecursiveSearch(bool isRecursive) {
        this->recursiveSearch = isRecursive;
    }

    void doScan(void);

    QList<Patient> getPatients(void) const;
    QList<Study> getStudies(const Patient &patient) const;
    QList<Series> getSeries(const Patient &patient, const Study &study) const;
    int getNumberOfImages(const Patient &patient, const Study &study, const Series &series) const;

    // XXX: Temp hack
    PatientMap getMap(void) const { return this->myMap; }

    void abortScanning(void);

signals:
    void reportProgress(unsigned int scannedFiles, unsigned int parsedFiles);

private:
    bool extract(QString path);

    QString path;
    bool recursiveSearch;
    bool abort;

    /* <Patient, <Study, <Series, QList<Path>>>> */
    QMap<Patient, QMap<Study, QMap<Series, QList<QString> > > > myMap;
};

#endif // PATIENTEXPLORER_H
