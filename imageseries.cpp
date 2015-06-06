#include "imageseries.h"

ImageSeries::ImageSeries()
{
}

void ImageSeries::addImage(const QString &path)
{
    this->vecImages.append(path);
}
