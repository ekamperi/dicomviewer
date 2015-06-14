#include "mymath.h"

#include <QtGlobal>
#include <QDebug>

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

void MyMath::getPointsInCircle(QVector<QPoint> *pVecPoints,
                                   QPoint centerPoint, float radius)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(pVecPoints);

    /* Calculate bounding box coordinates */
    /* Discard points with negative (offscreen) coordinates */
    int left   = qMax(0.0f, centerPoint.x() - radius);
    int top    = qMax(0.0f, centerPoint.y() - radius);
    int right  = centerPoint.x() + radius;
    int bottom = centerPoint.y() + radius;

    for (int x = left; x < right; x++) {
        for (int y = top; y < bottom; y++) {
            float dx = x - centerPoint.x();
            float dy = y - centerPoint.y();
            float distSquared = dx*dx + dy*dy;
            if (distSquared < radius*radius) {
                pVecPoints->push_back(QPoint(x, y));
            }
        }
    }
}

#define THRESH_SEL 1500 // Selection threshold, the more the less sensitive
void MyMath::getSelectedLines(const QVector<DistLine*> *pVecDistLines, const QPointF &currPoint)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(pVecDistLines);

    for (int i = 0; i < pVecDistLines->size(); i++) {
        DistLine *pLine = pVecDistLines->at(i);
        Q_ASSERT(pLine);

        bool onLine = false;
        bool betweenPoints = false;

        QPoint p1 = pLine->p1();
        QPoint p2 = pLine->p2();

        /* Check whether current point lies on line */
        int v1x = currPoint.x() - p1.x();
        int v1y = currPoint.y() - p1.y();
        int v2x = p2.x() - p1.x();
        int v2y = p2.y() - p1.y();
        qint64 cross = v1x * v2y - v2x * v1y;

        if (qAbs(cross) < THRESH_SEL) {
            onLine = true;
        }

        /* Check whether current point lies between the original points */
        if (abs(v2x) >= abs(v2y)) {
            betweenPoints =
                    v2x > 0 ?
            p1.x() <= currPoint.x() && currPoint.x() <= p2.x() :
            p2.x() <= currPoint.x() && currPoint.x() <= p1.x();
        } else {
          betweenPoints =
                  v2y > 0 ?
            p1.y() <= currPoint.y() && currPoint.y() <= p2.y() :
            p2.y() <= currPoint.y() && currPoint.y() <= p1.y();
        }

        /* If current point is on the line that p1, p2 define AND is
         * between p1 and p2, then we have a match.
         */
        if (onLine && betweenPoints) {
            bool isSelected = pLine->isSelected();
            pLine->setSelection(!isSelected); // toggle
        }
    }
}

void MyMath::getSelectedCircles(const QVector<DensCircle*> *pvDensCircles, const QPointF &currPoint)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(pvDensCircles);

    for (int i = 0; i < pvDensCircles->size(); i++) {
        DensCircle *pDensCircle = pvDensCircles->at(i);
        Q_ASSERT(pDensCircle);

        QPoint center = pDensCircle->center();
        int radius = pDensCircle->radius();

        int d = (currPoint.x() - center.x()) * (currPoint.x() - center.x()) +
                (currPoint.y() - center.y()) * (currPoint.y() - center.y()) -
                radius * radius;

        qDebug() << d;
        if (abs(d) < THRESH_SEL) {
            bool isSelected = pDensCircle->isSelected();
            pDensCircle->setSelection(!isSelected); // toggle
        }
    }
}
