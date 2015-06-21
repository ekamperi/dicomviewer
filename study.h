#ifndef STUDY_H
#define STUDY_H

#include "imageseries.h"
#include <QMetaType>

class Study
{
public:
    Study();
    Study(QString studyUID, QString studyDesc, QString studyDate);
    Study(const Study &obj);
    ~Study() {}

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

Q_DECLARE_METATYPE(Study)

#endif // STUDY_H
