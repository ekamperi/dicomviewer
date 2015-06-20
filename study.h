#ifndef STUDY_H
#define STUDY_H

#include "imageseries.h"

class Study
{
public:
    Study(QString studyUID, QString studyDesc, QString studyDate);

    QString getUID(void) const;
    QString getDesc(void) const;
    QString getDate(void) const;

    /* The key type of a QMap must provide operator<() specifying a total order. */
    bool operator<(const Study &study) const;

private:
    QString uid;
    QString desc;
    QString date;
};

#endif // STUDY_H
