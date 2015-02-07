#ifndef TOPOGRAM_H
#define TOPOGRAM_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class Topogram : public QWidget
{
    Q_OBJECT
public:
    explicit Topogram(QWidget *parent = 0);
    explicit Topogram(float *pData, int width, int height,
                      int sliceIndex, QWidget *parent = 0);
    ~Topogram();

    void setNewSliceIndex(unsigned int newSliceIndex);

protected:
    virtual void paintEvent(QPaintEvent *pEvent);
    virtual void mouseMoveEvent(QMouseEvent *pEvent);

signals:

public slots:

private:
    QVBoxLayout *pLayout;
    QLabel *pLabel;
    QImage* pImage;
    QPixmap *pPixmap;

    /* Raw data are the floating point original pixel values */
    float *pRawData;
    unsigned char *pConvertedData;

    /* */
    int rawWidth;
    int rawHeight;

    /* Current slice index that we are looking at */
    unsigned int sliceIndex;
    unsigned int totalSlices;

    /* Min and max values for the transfer function */
    float tmin;
    float tmax;
};

#endif // TOPOGRAM_H
