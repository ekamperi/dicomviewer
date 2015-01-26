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
    HounsFieldUnit(float slope, float intercept);
    QPair<float, float> getRawRange(float center, float width) const;
    QPair<float, float> getRawRange(HUWindows::window window) const;

private:
    float slope;
    float intercept;
    float maxPixel;
};
#endif // HOUNSFIELDUNIT_H
