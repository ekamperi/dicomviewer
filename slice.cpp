#include "slice.h"

#include "examdetails.h"

Slice::Slice(
        unsigned char *pRawPixelData,
        unsigned int width,
        unsigned int height,
        ExamDetails examDetails)
{
    this->pRawPixelData = pRawPixelData;
    this->width = width;
    this->height = height;
    this->examDetails = examDetails;
}
