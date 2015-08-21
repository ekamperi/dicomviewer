#include "examdetails.h"

#include "QDate"
#include "QtDebug"

QString ExamDetails::getStudyDate(void) const
{
    QString studyDate = this->details["studyDate"];

    /* Convert study date from e.g. 20120618 to 18-06-2012 */
    QDate myQDate = QDate::fromString(studyDate, "yyyyMMdd");
    return myQDate.toString("dd-MM-yyyy");
}

QString ExamDetails::getStudyTime(void) const
{
    QString studyTime = this->details["studyTime"];

    /* Convert study date from e.g. 113901 to 11:39:01 */
    QTime myQTime = QTime::fromString(studyTime, "hhmmss");
    return myQTime.toString("hh:mm:ss");
}

QString ExamDetails::getGenericDetails(void) const
{
    QString genericDetails;

    genericDetails =
            "Name: " + this->getPatientName()
            +        ", ID: " + this->getPatientID()  + "\n"
            +         "Age: " + this->getPatientAge() + "\n"
            +         "Sex: " + this->getPatientSex() + "\n"
            +        "Date: " + this->getStudyDate() + "\n"
            +        "Time: " + this->getStudyTime() + "\n"
            + "Institution: " + this->getInstitutionName();

    return genericDetails;
}

QString ExamDetails::getSpecificDetails(Exam::Modality examModality) const
{
    QString specificDetails;

    if (examModality == Exam::CT) {
        specificDetails =
           "Patient position: " + this->getPatientPosition() + "\n"
                +       "KVp: " + this->getKVP() + "\n"
                +  "Exposure: " + this->getExposure() + "\n"
                +      "Tilt: " + this->getGantryDetectorTilt() + "\n"
                + "Thickness: " + this->getSliceThickness() + "mm" + "\n"
                + "Slice Location: " + this->getSliceLocation();
    }

    return specificDetails;
}
