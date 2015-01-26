#ifndef HOUNSFIELDUNIT_H
#define HOUNSFIELDUNIT_H

#include <QPair>

namespace HUWindows {
    enum window {
        BONES,
        LUNG,
        MEDIASTINUM
    };
}

class HounsFieldUnit
{
public:
    HounsFieldUnit();
    HounsFieldUnit(float slope, float intercept);

    void setSlopeIntercept(float slope, float intercept) {
        this->slope = slope;
        this->intercept = intercept;
    }

    void setDefaultCenterWidth(float huCenter, float huWidth) {
        this->defCenterHU = huCenter;
        this->defWidthHU = huWidth;
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
