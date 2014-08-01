#ifndef SLICE_H
#define SLICE_H

#include "examdetails.h"
#include "GL/gl.h"

class Slice
{
public:
    Slice(unsigned char *pRawPixelData, unsigned int width, unsigned int height, GLint format, ExamDetails examDetails);

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

private:
    unsigned char *pRawPixelData;
    unsigned int width;
    unsigned int height;
    GLint format;

    ExamDetails examDetails;
};

#endif // SLICE_H
