#ifndef PATIENTEXPLORER_H
#define PATIENTEXPLORER_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QObject>

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

    QList<QString> getPatients(void) const;
    QList<Study> getStudies(const QString &patientName) const;
    QList<Series> getSeries(const QString &patientName, const Study &study) const;

    void abortScanning(void);

signals:
    void reportProgress(unsigned int scannedFiles);

private:
    void extract(QString path);

    QString path;
    bool abort;

    /* <Patient, <Study, <Series, Path>>> */
    QMap<QString, QMap<Study, QMap<Series, QString> > > myMap;
};

#endif // PATIENTEXPLORER_H
