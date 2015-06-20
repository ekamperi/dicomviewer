#include "study.h"

#include <QDate>

Study::Study(QString studyUID, QString studyDesc, QString studyDate)
{
    this->uid = studyUID;
    this->desc = studyDesc;
    this->date = studyDate;
}

QString Study::getUID(void) const
{
    return this->uid;
}

QString Study::getDesc(void) const
{
    return this->desc;
}

QString Study::getDate(void) const
{
    /* Convert study date from e.g. 20120618 to 18-06-2012 */
    QDate myQDate = QDate::fromString(this->date, "yyyyMMdd");
    return myQDate.toString("dd-MM-yyyy");
}

/* The key type of a QMap must provide operator<() specifying a total order. */
bool Study::operator<(const Study &study) const
{
    return this->uid < study.getUID();
}
