#ifndef STUDY_H
#define STUDY_H

#include "imageseries.h"

class Study
{
public:
    Study();
    void addImageSeries(const ImageSeries &imageSeries);

private:
    QVector<ImageSeries> vecImageSeries;
};

#endif // STUDY_H
