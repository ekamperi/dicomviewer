#ifndef SLICE_H
#define SLICE_H

#include "examdetails.h"

class Slice
{
public:
    Slice(unsigned char *pRawPixelData, unsigned int width, unsigned int height, ExamDetails examDetails);

    unsigned char *getRawPixelData() {
        return this->pRawPixelData;
    }
    unsigned int getWidth() {
        return this->width;
    }
    unsigned int getHeight() {
        return this->height;
    }
    ExamDetails getExamDetails(void)
    {
        return this->examDetails;
    }

private:
    unsigned char *pRawPixelData;
    unsigned int width;
    unsigned int height;

    ExamDetails examDetails;
};

#endif // SLICE_H
