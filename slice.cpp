#include "slice.h"

Slice::Slice(unsigned char *pRawPixelData, unsigned int width, unsigned int height)
{
    this->pRawPixelData = pRawPixelData;
    this->width = width;
    this->height = height;
}
