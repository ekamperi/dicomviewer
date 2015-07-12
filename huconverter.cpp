#include "huconverter.h"
#include "math.h"

#include <QtDebug>
#include <QtGlobal>

HUConverter::HUConverter()
{
//    qDebug() << Q_FUNC_INFO;
    this->slope = 0.0;
    this->intercept = 0.0;
    this->maxPixel = 1.0;
}

HUConverter::HUConverter(float slope, float intercept)
{
//    qDebug() << Q_FUNC_INFO;
    this->slope = slope;
    this->intercept = intercept;
}

QPair<float, float> HUConverter::getRawRange(float huCenter, float huWidth) const
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "huCenter = " << huCenter << "huWidth = " << huWidth << "maxPixel = " << this->maxPixel;
    qDebug() << "intercept = " << this->intercept;
    qDebug() << "slope = " << this->slope;

    float rawCenter = (huCenter - this->intercept) / this->slope;
    float rawWidth = (huWidth - this->intercept) / this->slope;

    Q_ASSERT(!isnan(rawCenter));
    Q_ASSERT(!isnan(rawWidth));

    /* Lowest and highest visible pixels */
    float lvv = rawCenter - rawWidth / 2;
    float hvv = rawCenter + rawWidth / 2;

    lvv = lvv / this->maxPixel;
    hvv = hvv / this->maxPixel;

    return QPair<float, float>(lvv, hvv);
}

QPair<float, float>
HUConverter::getDefaultNormalizedRange(void) const
{
    qDebug() << Q_FUNC_INFO;

    return this->getRawRange(this->defCenterHU, this->defWidthHU);
}

QPair<float, float>
HUConverter::getNormalizedRangeFromTemplate(HUWindows::window window) const
{
    qDebug() << Q_FUNC_INFO;

    float huCenter;
    float huWidth;

    if (window == HUWindows::ABDOMEN) {
        huCenter = 60;
        huWidth = 400;
    } else if (window == HUWindows::BONE) {
        huCenter = 300;
        huWidth = 1500;
    } else if (window == HUWindows::HEAD) {
        huCenter = 40;
        huWidth = 80;
    } else if (window == HUWindows::LUNG) {
        huCenter = -600;
        huWidth = 1700;
    } else if (window == HUWindows::MEDIASTINUM) {
        huCenter =-50;
        huWidth = 400;
    } else {
        qDebug() << "BUG:" << window;
        huCenter = -50;
        huWidth = 400;
        //Q_ASSERT(false);
    }

    return this->getRawRange(huCenter, huWidth);
}

void HUConverter::normalize(float maxPixel)
{
//    qDebug() << Q_FUNC_INFO;

    this->maxPixel = maxPixel;
}

float HUConverter::rawToHUs(float pixelValue)
{
    qDebug() << Q_FUNC_INFO;

    /* HUs = value * slope + intercept */
    float huValue = pixelValue * this->slope + this->intercept;

    return huValue;
}
