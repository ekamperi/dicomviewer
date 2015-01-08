#ifndef SLICE_H
#define SLICE_H

#include "dicomfile.h"
#include "examdetails.h"
#include "myglwidget.h"
#include "GL/gl.h"

class MyGLWidget;

class Slice
{
public:
    Slice(QString filename, unsigned int index);
    ~Slice();

    unsigned int getIndex() const {
        return this->index;
    }

    unsigned char *getRawPixelData() const {
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

    void setGLWidget(MyGLWidget *pMyGLWidget) {
        this->pMyGLWidget = pMyGLWidget;
    }

    MyGLWidget *getGLWidget(void) const {
        return this->pMyGLWidget;
    }

private:
    DicomFile *pDicomFile;
    unsigned int index;

    unsigned char *pRawPixelData;
    unsigned int width;
    unsigned int height;
    GLint format;

    ExamDetails examDetails;

    /* Whether this slice was selected by user */
    bool m_isSelected;

    MyGLWidget *pMyGLWidget;
};

#endif // SLICE_H
