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
    explicit Topogram(const QVector<Slice *> *pVecSlices, float angle,
                      int sliceIndex, QWidget *parent = 0);
    ~Topogram();

    void setNewSliceIndex(unsigned int newSliceIndex);
    bool isEmbedded(void) const { return this->amIEmbedded; }
    void setEmbedded(bool value) { this->amIEmbedded = value; }

protected:
    virtual void paintEvent(QPaintEvent *pEvent);
    virtual void mouseMoveEvent(QMouseEvent *pEvent);
    virtual void mousePressEvent(QMouseEvent *pEvent);
    virtual void mouseReleaseEvent(QMouseEvent *pEvent);

signals:
    void iWantToUndock(void);
    void iWantToDock(void);
    void sliceChanged(int newSliceIndex);

private slots:
    void showContextMenu(const QPoint &pos);

private:
    QImage* pImage;

    void genRawData(void);
    void genImage(void);
    void regenImage(void);

    /* Raw data are the floating point original pixel values */
    float *pRawData;

    /* Converted data are floating point pixel data [0.0, 1.0] converted
     * to unsigned char [0, 255] */
    unsigned char *pConvertedData;

    /* Default color table */
    QVector<QRgb> colorTable;

    /* */
    int rawWidth;
    int rawHeight;

    /* We need the slices to access their raw pixel data (among other things) */
    const QVector<Slice *> *pVecSlices;

    /* Current angle of view in radians */
    float angle;

    /* Current slice index that we are looking at */
    unsigned int sliceIndex;
    unsigned int totalSlices;

    /* Min and max values for the transfer function */
    float tmin;
    float tmax;

    /* Whether topogram is an autonomous window or embedded into the GLWidget */
    bool amIEmbedded;

    bool intensity;

};

#endif // TOPOGRAM_H
