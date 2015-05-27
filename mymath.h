#ifndef MYMATH_H
#define MYMATH_H

#include <QVector>
#include <QPoint>

class MyMath
{
public:
    MyMath();
    ~MyMath();

    static float cclamp(float x, float minVal, float maxVal);
    static float sstep(float edge0, float edge1, float x);

    static unsigned char *floatToByte(float *pRawData, int width, int height);
    static unsigned char *floatToByte(float *pRawData, int width, int height, float tmin, float tmax);

    static void getPointsInCircle(QVector<QPoint> *pVecPoints, QPoint centerPoint, float radius);
};

#endif // MYMATH_H
