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
    void extract(QString path);

    QMap<QString, QMap<QString, QString> > myMap;
};

#endif // PATIENTEXPLORER_H
