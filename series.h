#ifndef SERIES_H
#define SERIES_H

#include <QString>

class Series
{
public:
    Series(QString seriesUID, QString seriesDesc, QString seriesDate);

    QString getUID(void) const;
    QString getDesc(void) const;
    QString getDate(void) const;

    /* The key type of a QMap must provide operator<() specifying a total order. */
    bool operator<(const Series &series) const;

private:
    QString uid;
    QString desc;
    QString date;
};

#endif // SERIES_H
