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

    bool isSelected(void) const {
        return this->m_isSelected;
    }

    void setSelected(bool selected) {
        this->m_isSelected = selected;
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

    void normalizePixels(float globalMaxPixel);

    void setWindow(HUWindows::window huWindow);

    QPair<float, float> getWindowLevelWidth(void) const;
    DicomFile *pDicomFile;

signals:
    void iNeedRepaint(float tmin, float tmax);

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
    HUWindows::window huWindow;

    /* Whether this slice was selected by user. If yes, we draw a red outline  */
    bool m_isSelected;

    /* MyImageWidget is used in the grid layout */
    ImageWidget *pMyImageWidget;
};

#endif // SLICE_H
