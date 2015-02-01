#include "examdetails.h"

#include "QDate"

QString ExamDetails::getStudyDate(void) const
{
    QString studyDate = this->details["studyDate"];

    /* Convert study date from e.g. 20120618 to 18-06-2012 */
    QDate myQDate = QDate::fromString(studyDate, "yyyyMMdd");
    return myQDate.toString("dd-MM-yyyy");
}
