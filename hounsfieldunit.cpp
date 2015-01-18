#include "hounsfieldunit.h"
#include "math.h"

#include <QtDebug>

HounsFieldUnit::HounsFieldUnit(float slope, float intercept)
{
    this->slope = slope;
    this->intercept = intercept;
}

QPair<float, float> HounsFieldUnit::getRawRange(float huCenter, float huWidth) const
{
    float rawCenter = (huCenter - this->intercept) / slope;
    float rawWidth = (huWidth - this->intercept) / slope;

    Q_ASSERT(!isnan(rawCenter));
    Q_ASSERT(!isnan(rawWidth));

    /* Lowest and highest visible pixels */
    float lvv = rawCenter - rawWidth / 2;
    float hvv = rawCenter + rawWidth / 2;

    //if (lvv < 0) lvv = 0;
    //if (hvv > 4096) hvv = 4096;

    return QPair<float, float>(lvv, hvv);
}

QPair<float, float> HounsFieldUnit::getRawRange(HUWindows::window window) const
{
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
