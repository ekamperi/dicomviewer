#ifndef IMAGESERIES_H
#define IMAGESERIES_H

#include <QString>
#include <QVector>

class ImageSeries
{
public:
    ImageSeries();
    void addImage(const QString &path);

private:
    QVector<QString> vecImages;
};

#endif // IMAGESERIES_H
