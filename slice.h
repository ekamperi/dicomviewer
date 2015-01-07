#ifndef SLICE_H
#define SLICE_H

#include "dicomfile.h"
#include "examdetails.h"
#include "GL/gl.h"

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

private:
    DicomFile *pDicomFile;
    unsigned int index;

    unsigned char *pRawPixelData;
    unsigned int width;
    unsigned int height;
    GLint format;

    ExamDetails examDetails;
};

#endif // SLICE_H
