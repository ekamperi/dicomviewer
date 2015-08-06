#include "series.h"

#include <QDate>
#include <QDebug>

Series::Series()
{
    this->uid = "Series default";
    this->desc = "Series default";
    this->date = "Series default";
    this->time = "Series default";
    this->modality = "Series default";
}

Series::Series(QString seriesUID, QString seriesDesc, QString seriesDate,
               QString seriesTime, QString modality, int numOfImages)
{
    this->uid = seriesUID;
    this->desc = seriesDesc;
    this->modality = modality;
    this->numOfImages = numOfImages;

    /* Convert series date from e.g. 20120618 to 18-06-2012 */
    QDate myQDate = QDate::fromString(seriesDate, "yyyyMMdd");
    this->date = myQDate.toString("dd-MM-yyyy");

    QTime myQTime = QTime::fromString(seriesTime.left(6), "hhmmss");
    if (!myQTime.isValid()) {
        qDebug() << Q_FUNC_INFO << "Time was not valid: " << seriesTime.left(6);
    }
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

QString Series::getModality(void) const
{
    return this->modality;
}

int Series::getNumOfImages(void) const
{
    return this->numOfImages;
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
    this->modality = obj.getModality();
    this->numOfImages = obj.getNumOfImages();
}
