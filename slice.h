#ifndef SLICE_H
#define SLICE_H

#include "dicomfile.h"
#include "examdetails.h"
#include "include/helpers/huconverter.h"
#include "GL/gl.h"

#include <QObject>

class MyGLWidget;
class ImageWidget;

class Slice : public QObject
{
    Q_OBJECT

public:
    Slice(QString filename, unsigned int index);
    ~Slice();

    unsigned int getIndex(void) const {
        return this->index;
    }

    float *getRawPixelData(void) const {
        return this->pRawPixelData;
    }

    unsigned int getWidth(void) const {
        return this->width;
    }

    unsigned int getHeight(void) const {
        return this->height;
    }

    GLint getFormat(void) const {
        return this->format;
    }

    ExamDetails getExamDetails(void) const  {
        return this->examDetails;
    }

    void setImageWidget(ImageWidget *pMyImageWidget) {
        this->pMyImageWidget = pMyImageWidget;
    }

    ImageWidget *getImageWidget(void) const {
        return this->pMyImageWidget;
    }

    float getMaxPixel(void) const {
        return this->maxPixel;
    }

    float getGlobalMaxPixel(void) const {
        return this->globalMaxPixel;
    }

    const HUConverter *getHUConverter(void) const {
        return this->pHUConverter;
    }

    void normalizePixels(float globalMaxPixel);

    QPair<float, float> getDefaultWindowLevelWidth(void) const;
    DicomFile *pDicomFile;

private:
    unsigned int index;

    float *pRawPixelData;
    unsigned int width;
    unsigned int height;
    GLint format;

    float maxPixel;

    /* Global max pixel (across all slices) */
    float globalMaxPixel;

    /* Name, sex, age, etc */
    ExamDetails examDetails;

    /* Default window/width in Hounsfield Units, e.g. 40/400 */
    HUConverter *pHUConverter;

    /* MyImageWidget is used in the grid layout */
    ImageWidget *pMyImageWidget;
};

#endif // SLICE_H
