#ifndef SLICE_H
#define SLICE_H

class Slice
{
public:
    Slice(unsigned char *pRawPixelData, unsigned int width, unsigned int height);

    unsigned char *getRawPixelData() {
        return this->pRawPixelData;
    }
    unsigned int getWidth() {
        return this->width;
    }
    unsigned int getHeight() {
        return this->height;
    }

private:
    unsigned char *pRawPixelData;
    unsigned int width;
    unsigned int height;
};

#endif // SLICE_H
