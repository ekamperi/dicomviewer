#ifndef HOUNSFIELDUNIT_H
#define HOUNSFIELDUNIT_H

#include <QPair>

namespace HUWindows {
    enum window {
        ABDOMEN,
        BONE,
        HEAD,
        LUNG,
        MEDIASTINUM,
        SOFT_TISSUE
    };
}

class HUConverter
{
public:
    HUConverter();
    HUConverter(float slope, float intercept);
    float rawToHUs(float pixelValue);

    void setSlopeIntercept(float slope, float intercept) {
        this->slope = slope;
        this->intercept = intercept;
    }

    void setDefaultCenterWidth(float huCenter, float huWidth) {
        this->defCenterHU = huCenter;
        this->defWidthHU = huWidth;
    }

    float getIntercept(void) const
    {
        return this->intercept;
    }

    float getSlope(void) const
    {
        return this->slope;
    }

    void normalize(float maxPixel);

    QPair<float, float> getDefaultNormalizedRange(void) const;
    QPair<float, float> getRawRange(float center, float width) const;
    QPair<float, float> getNormalizedRangeFromTemplate(HUWindows::window window) const;

private:
    float slope;
    float intercept;

    float defCenterHU;
    float defWidthHU;

    float maxPixel;
};
#endif // HOUNSFIELDUNIT_H
