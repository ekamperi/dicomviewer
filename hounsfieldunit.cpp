#include "hounsfieldunit.h"
#include "math.h"

#include <QtDebug>
#include <QtGlobal>

HounsFieldUnit::HounsFieldUnit()
{
    qDebug() << Q_FUNC_INFO;
    this->slope = 0.0;
    this->intercept = 0.0;
    this->maxPixel = 1.0;
}

HounsFieldUnit::HounsFieldUnit(float slope, float intercept)
{
    qDebug() << Q_FUNC_INFO;
    this->slope = slope;
    this->intercept = intercept;
}

QPair<float, float> HounsFieldUnit::getRawRange(float huCenter, float huWidth) const
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "huCenter = " << huCenter << "huWidth = " << huWidth << "maxPIxel = " << this->maxPixel;
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

    return QPair<float, float>(0, 1);
}

QPair<float, float>
HounsFieldUnit::getDefaultNormalizedRange(void) const
{
    qDebug() << Q_FUNC_INFO;

    return this->getRawRange(this->defCenterHU, this->defWidthHU);
}

QPair<float, float>
HounsFieldUnit::getNormalizedRangeFromTemplate(HUWindows::window window) const
{
    qDebug() << Q_FUNC_INFO;

    float huCenter;
    float huWidth;

    if (window == HUWindows::BONES) {
        huCenter = 1000;
        huWidth = 2500;
    } else if (window == HUWindows::LUNG) {
        huCenter = -600;
        huWidth = 1700;
    } else if (window == HUWindows::MEDIASTINUM) {
        huCenter =-50;
        huWidth = 400;
    } else {
        Q_ASSERT(false);
    }

    return this->getRawRange(huCenter, huWidth);
}

void HounsFieldUnit::normalize(float maxPixel)
{
    qDebug() << Q_FUNC_INFO;

    this->maxPixel = maxPixel;
}
