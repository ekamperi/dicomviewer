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
    return t*t* (3.0 - 2.0*t);
}
