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

class PatientExplorer
{
public:
    PatientExplorer(QString path);

private:
    void getPatientName(QString path);

    QMap<QString, QMap<QString, QString> > myMap;

    DcmFileFormat dcmFile;
};

#endif // PATIENTEXPLORER_H
