#ifndef MYIMAGEWIDGET_H
#define MYIMAGEWIDGET_H

#include <QLabel>
#include <QWidget>

#include "slice.h"

class ImageWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = 0);
    ~ImageWidget();

    void setSlice(Slice *pSlice);

private:
    void genImage();
    void drawOutline(QPainter *pPainter);

    /* Whether we are inside a slice or not, used in hover effect */
    bool weAreIn;

    Slice *pSlice;
    QImage *pImage;

    /* This color table is used for converting a 4096 tones gray scale
     * image to 256 color (8bit) gray scale image.
     */
    QVector<QRgb> myColorTable;
    unsigned char *pData;   // 8bit data

protected:
    void paintEvent(QPaintEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    /* These events are used for implementing a hover effect */
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

signals:
    void sliceDoubleClicked(const Slice *pSlice);
    void sliceIndexChanged(unsigned int sliceIndex);

private slots:
    void changeWindow(float, float);
};

#endif // MYIMAGEWIDGET_H