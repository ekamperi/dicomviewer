#include "study.h"

#include <QDate>
#include <QDebug>

Study::Study()
{
    this->uid = "Study default";
    this->desc = "Study default";
    this->date = "Study default";
    this->time = "Study default";
}

Study::Study(QString studyUID, QString studyDesc, QString studyDate, QString studyTime)
{
    this->uid = studyUID;
    this->desc = studyDesc;

    /* Convert study date from e.g. 20120618 to 18-06-2012 */
    QDate myQDate = QDate::fromString(studyDate, "yyyyMMdd");
    this->date = myQDate.toString("dd-MM-yyyy");

    QTime myQTime = QTime::fromString(studyTime, "hhmmss");
    //Q_ASSERT(myQTime.isValid());
    this->time = myQTime.toString("hh:mm:ss");
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
    return this->date;
}

QString Study::getTime(void) const
{
    return this->time;
}

/* The key type of a QMap must provide operator<() specifying a total order. */
bool Study::operator<(const Study &study) const
{
    return this->uid < study.getUID();
}

Study::Study(const Study &obj)
{
    this->uid = obj.getUID();
    this->desc = obj.getDesc();
    this->date = obj.getDate();
    this->time = obj.getTime();
}
