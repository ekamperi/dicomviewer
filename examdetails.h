#ifndef EXAMDETAILS_H
#define EXAMDETAILS_H

#include <QMap>
#include <QString>

class ExamDetails
{
public:
    ExamDetails() {}

    ExamDetails(QMap<QString, QString> details) {
        this->details = details;
    }

    QString getPatientName(void) const {
        return this->details["patientName"];
    }

    QString getPatientID(void) const {
        return this->details["patientID"];
    }

    QString getPatientAge(void) const {
        return this->details["patientAge"];
    }

    QString getPatientSex(void) const {
        return this->details["patientSex"];
    }

    QString getPatientBirthDate(void) const {
        return this->details["patientBirthDate"];
    }

    QString getStudyDate(void) const;

    QString getStudyTime(void) const;

    QMap<QString, QString> getDetails(void) {
        return this->details;
    }

private:
    QMap<QString, QString> details;
};

#endif // EXAMDETAILS_H
