#ifndef PATIENT_H
#define PATIENT_H

#include <QMetaType>
#include <QString>

class Patient
{
public:
    Patient();
    Patient(const QString &name);
    Patient(const Patient &obj);
    ~Patient() {}

    QString getName(void) const;

    /* The key type of a QMap must provide operator<() specifying a total order. */
    bool operator<(const Patient &patient) const;
private:
    QString name;
};

Q_DECLARE_METATYPE(Patient)

#endif // PATIENT_H
