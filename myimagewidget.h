#ifndef MYIMAGEWIDGET_H
#define MYIMAGEWIDGET_H

#include <QLabel>
#include <QWidget>

#include "slice.h"

class MyImageWidget : public QLabel
{
    Q_OBJECT
public:
    explicit MyImageWidget(QWidget *parent = 0);
    ~MyImageWidget();

    void setSlice(Slice *pSlice);

private:
    void loadTexture(float *pRawData, unsigned int width, unsigned int height, GLint format);
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
    void sliceDoubleClicked(Slice *pSlice);

public slots:
};

#endif // MYIMAGEWIDGET_H
