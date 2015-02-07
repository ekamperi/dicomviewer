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
    explicit Topogram(float *pData, int width, int height, int sliceIndex, QWidget *parent = 0);
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
    float *pRawData;

    /* Current slice index that we are looking at */
    unsigned int sliceIndex;
    unsigned int totalSlices;

    float tmin;
    float tmax;
};

#endif // TOPOGRAM_H
