#include "slice.h"

#include "examdetails.h"

Slice::Slice(
        unsigned char *pRawPixelData,
        unsigned int width,
        unsigned int height,
        GLint format,
        ExamDetails examDetails)
{
    this->pRawPixelData = pRawPixelData;
    this->width = width;
    this->height = height;
    this->format = format;
    this->examDetails = examDetails;
}
