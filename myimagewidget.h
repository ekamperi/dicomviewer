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
    void loadTexture(Uint8 *pRawData, unsigned int width, unsigned int height, GLint format);
    void drawOutline(QPainter *pPainter);

    /* Whether we are inside a slice or not */
    bool weAreIn;

    Slice *pSlice;
    QImage *pImage;
    QVector<QRgb> myColorTable;

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
