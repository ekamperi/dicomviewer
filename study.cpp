#include "study.h"

Study::Study()
{
}

void Study::addImageSeries(const ImageSeries &imageSeries)
{
    this->vecImageSeries.append(imageSeries);
}
