#ifndef TOPOGRAM_H
#define TOPOGRAM_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "slice.h"

class Topogram : public QWidget
{
    Q_OBJECT
public:
    explicit Topogram(QWidget *parent = 0);
    explicit Topogram(const QVector<Slice *> *pVecSlices, float angle, int width, int height,
                      int sliceIndex, QWidget *parent = 0);
    ~Topogram();

    void setNewSliceIndex(unsigned int newSliceIndex);
    bool isEmbedded(void) const { return this->amIEmbedded; }
    void setEmbedded(bool value) { this->amIEmbedded = value; }

protected:
    virtual void paintEvent(QPaintEvent *pEvent);
    virtual void mouseMoveEvent(QMouseEvent *pEvent);
    virtual void mouseReleaseEvent(QMouseEvent *pEvent);

signals:
    void iWantToBreakFree(void);
    void sliceChanged(int newSliceIndex);

public slots:

private:
    QVBoxLayout *pLayout;
    QLabel *pLabel;
    QImage* pImage;
    QPixmap *pPixmap;

    void genData(void);
    void regenData(void);   // regenData() just calls genData(). It's used for clarity.

    void genImage(void);

    /* Raw data are the floating point original pixel values */
    float *pRawData;
    unsigned char *pConvertedData;

    /* */
    int rawWidth;
    int rawHeight;

    /* */
    const QVector<Slice *> *pVecSlices;

    /* Current angle in radians from where we are looking at */
    float angle;

    /* Current slice index that we are looking at */
    unsigned int sliceIndex;
    unsigned int totalSlices;

    /* Min and max values for the transfer function */
    float tmin;
    float tmax;

    /* Whether topogram is an autonomous window or embedded into the GLWidget */
    bool amIEmbedded;
};

#endif // TOPOGRAM_H
