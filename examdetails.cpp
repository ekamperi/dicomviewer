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
