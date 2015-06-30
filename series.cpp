#include "series.h"

#include <QDate>

Series::Series()
{
    this->uid = "Series default";
    this->desc = "Series default";
    this->date = "Series default";
    this->time = "Series default";
}

Series::Series(QString seriesUID, QString seriesDesc, QString seriesDate, QString seriesTime)
{
    this->uid = seriesUID;
    this->desc = seriesDesc;

    /* Convert series date from e.g. 20120618 to 18-06-2012 */
    QDate myQDate = QDate::fromString(seriesDate, "yyyyMMdd");
    this->date = myQDate.toString("dd-MM-yyyy");

    QTime myQTime = QTime::fromString(seriesTime, "hhmmss");
   // Q_ASSERT(myQTime.isValid());
    this->time = myQTime.toString("hh:mm:ss");
}

QString Series::getUID(void) const
{
    return this->uid;
}

QString Series::getDesc(void) const
{
    return this->desc;
}

QString Series::getDate(void) const
{
    return this->date;
}

QString Series::getTime(void) const
{
    return this->time;
}

/* The key type of a QMap must provide operator<() specifying a total order. */
bool Series::operator<(const Series &series) const
{
    return this->uid < series.getUID();
}

Series::Series(const Series &obj)
{
    this->uid = obj.getUID();
    this->desc = obj.getDesc();
    this->date = obj.getDate();
    this->time = obj.getTime();
}
