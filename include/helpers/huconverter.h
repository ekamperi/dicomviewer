#ifndef HOUNSFIELDUNIT_H
#define HOUNSFIELDUNIT_H

#include <QDebug>
#include <QPair>
#include <QString>

namespace HUWindows {
    enum window {
        ABDOMEN,
        BONE,
        HEAD,
        LUNG,
        MEDIASTINUM,
        SOFT_TISSUE,
        UNKNOWN
    };
}

class HUWindow
{
public:
    static HUWindows::window fromText(QString window) {
        qDebug() << Q_FUNC_INFO;

        QString str = window.toLower();
        if (str == "abdomen") {
            return HUWindows::ABDOMEN;
        } else if (str == "bone") {
            return HUWindows::BONE;
        } else if (str == "head") {
            return HUWindows::HEAD;
        } else if (str == "lung") {
            return HUWindows::LUNG;
        } else if (str == "mediastinum") {
            return HUWindows::MEDIASTINUM;
        } else if (str == "soft_tissue") {
            return HUWindows::SOFT_TISSUE;
        } else {
            qDebug() << "Unknown CT window";
            return HUWindows::UNKNOWN;
        }
    }
};

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
