#ifndef EXAMDETAILS_H
#define EXAMDETAILS_H

#include <QMap>
#include <QString>

namespace Exam {
    enum Modality {
        CT,
        MRI
    };
}

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

    QString getInstitutionName(void) const {
        return this->details["institutionName"];
    }

    QString getStudyDate(void) const;
    QString getStudyTime(void) const;

    /* CT specific functions */
    QString getKVP(void) const {
        return this->details["kvp"];
    }

    QString getXRayTubeCurrent(void) const {
        return this->details["xrayTubeCurrent"];
    }

    QString getExposure(void) const {
        return this->details["exposure"];
    }

    QString getGantryDetectorTilt(void) const {
        return this->details["gantryDetectorTilt"];
    }

    QString getSliceThickness(void) const {
        return this->details["sliceThickness"];
    }

    /* Aggregate functions */
    QMap<QString, QString> getDetails(void) {
        return this->details;
    }

    QString getGenericDetails(void) const;
    QString getSpecificDetails(Exam::Modality examModality) const;

private:
    QMap<QString, QString> details;
};

#endif // EXAMDETAILS_H
