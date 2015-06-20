#include "series.h"

#include <QDate>

Series::Series(QString seriesUID, QString seriesDesc, QString seriesDate)
{
    this->uid = seriesUID;
    this->desc = seriesDesc;
    this->date = seriesDate;
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
    /* Convert series date from e.g. 20120618 to 18-06-2012 */
    QDate myQDate = QDate::fromString(this->date, "yyyyMMdd");
    return myQDate.toString("dd-MM-yyyy");
}

/* The key type of a QMap must provide operator<() specifying a total order. */
bool Series::operator<(const Series &series) const
{
    return this->uid < series.getUID();
}
