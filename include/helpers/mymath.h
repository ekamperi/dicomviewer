#ifndef MYMATH_H
#define MYMATH_H

#include <QVector>
#include <QPoint>

#include "distline.h"
#include "denscircle.h"

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
    static void getSelectedLines(const QVector<DistLine*> *pVecDistLines, const QPointF &currPoint);
    static void getSelectedCircles(const QVector<DensCircle*> *pvDensCircles, const QPointF &currPoint);
};

#endif // MYMATH_H
