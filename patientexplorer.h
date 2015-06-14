#ifndef PATIENTEXPLORER_H
#define PATIENTEXPLORER_H

#include <QMap>
#include <QString>
#include <QVector>

#include "study.h"

/* XXX: This is needed for compile */
#define HAVE_CONFIG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

typedef QMap<QString, QMap<QString, QString> >  StudyMap;

class PatientExplorer
{
public:
    PatientExplorer(QString path);

    QList<QString> getPatients(void);
    QList<QString> getStudies(const QString &patientName);

private:
    void extract(QString path);

    /* <Patient, <Study, <Series, Path>>> */
    QMap<QString, QMap<QString, QMap<QString, QString> > > myMap;
};

#endif // PATIENTEXPLORER_H
