#ifndef SLICE_H
#define SLICE_H

#include "dicomfile.h"
#include "examdetails.h"
#include "hounsfieldunit.h"
#include "GL/gl.h"

class MyGLWidget;
class MyImageWidget;

class Slice
{
public:
    Slice(QString filename, unsigned int index);
    ~Slice();

    unsigned int getIndex() const {
        return this->index;
    }

    float *getRawPixelData() const {
        return this->pRawPixelData;
    }

    unsigned int getWidth() const {
        return this->width;
    }

    unsigned int getHeight() const {
        return this->height;
    }

    GLint getFormat() const {
        return this->format;
    }

    ExamDetails getExamDetails(void) const  {
        return this->examDetails;
    }

    bool isSelected(void) const {
        return this->m_isSelected;
    }

    void setSelected(bool selected) {
        this->m_isSelected = selected;
    }

    void setImageWidget(MyImageWidget *pMyImageWidget) {
        this->pMyImageWidget = pMyImageWidget;
    }

    MyImageWidget *getImageWidget(void) const {
        return this->pMyImageWidget;
    }

    float getMaxPixel(void) const {
        return this->maxPixel;
    }

    void normalizePixels(float maxPixel);

    QPair<float, float> getDefaultWindowWidth(void) const;

private:
    DicomFile *pDicomFile;
    unsigned int index;

    float *pRawPixelData;
    unsigned int width;
    unsigned int height;
    GLint format;
    float maxPixel;

    ExamDetails examDetails;

    /* Default window/width in Hounsfield Units, e.g. 40/400 */
    HounsFieldUnit defHUWindowWidth;

    /* Whether this slice was selected by user */
    bool m_isSelected;

    MyImageWidget *pMyImageWidget;
};

#endif // SLICE_H
