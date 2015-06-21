#include "patient.h"

Patient::Patient()
{
    this->name = "Patient default";
}

Patient::Patient(const QString &name)
{
    this->name = name;
}

QString Patient::getName(void) const
{
    return this->name;
}

/* The key type of a QMap must provide operator<() specifying a total order. */
bool Patient::operator<(const Patient &patient) const
{
    return this->name < patient.getName();
}

Patient::Patient(const Patient &obj)
{
    this->name = obj.getName();
}
