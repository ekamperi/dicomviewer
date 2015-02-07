#include "mymath.h"

#include <QtGlobal>

MyMath::MyMath()
{

}

MyMath::~MyMath()
{

}

float MyMath::cclamp(float x, float minVal, float maxVal)
{
    return qMin(qMax(x, minVal), maxVal);
}

float MyMath::sstep(float edge0, float edge1, float x)
{
    float t = cclamp((x-edge0) / (edge1-edge0), 0.0, 1.0);
    return t*t*(3.0-2.0*t);
}

/* Convert float data [0.0, 1.0] to unsigned char [0, 255] */
/* The caller needs to free the memory */
unsigned char *MyMath::floatToByte(float *pRawData, int width, int height)
{
    Q_ASSERT(pRawData);

    unsigned long nPixels = width * height;
    unsigned char *pNewData = new unsigned char[nPixels];
    Q_ASSERT(pNewData);

    for (unsigned long i = 0; i < nPixels; i++) {
        pNewData[i] = (unsigned char) (pRawData[i] * 255);
    }

    return pNewData;
}

unsigned char *MyMath::floatToByte(float *pRawData, int width, int height, float tmin, float tmax)
{
    Q_ASSERT(pRawData);

    float *pTransferedData = new float[width * height];
    Q_ASSERT(pTransferedData);

    for (long i = 0; i < width * height; i++) {
        pTransferedData[i] = MyMath::sstep(tmin, tmax, pRawData[i]);
    }

    unsigned char *pNewData = MyMath::floatToByte(pTransferedData, width, height);
    delete pTransferedData;

    return pNewData;
}
