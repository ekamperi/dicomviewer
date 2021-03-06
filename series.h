#ifndef SERIES_H
#define SERIES_H

#include <QMetaType>
#include <QString>

class Series
{
public:
    Series();
    Series(QString seriesUID, QString seriesDesc, QString seriesDate,
           QString seriesTime, QString modality, int numOfImages);
    Series(const Series &obj);
    ~Series() {}

    QString getUID(void) const;
    QString getDesc(void) const;
    QString getDate(void) const;
    QString getTime(void) const;
    QString getModality(void) const;
    int getNumOfImages(void) const;

    /* The key type of a QMap must provide operator<() specifying a total order. */
    bool operator<(const Series &series) const;

private:
    QString uid;
    QString desc;
    QString date;
    QString time;
    QString modality;
    int numOfImages;
};

Q_DECLARE_METATYPE(Series)

#endif // SERIES_H
