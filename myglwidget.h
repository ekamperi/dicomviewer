#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <qt5/QtOpenGL/QGLWidget>

#include "examdetails.h"
#include "slice.h"
#include "Magick++.h"

class Slice;

class MyGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent = 0);
     ~MyGLWidget();

     void loadTextureFile(QString filename);
     void loadTextureFile2(unsigned char *pRawData, unsigned int width, unsigned int height, GLint format);

     void setSlice(Slice *pSlice);
     const Slice * getSlice() const;
     unsigned int getSliceIndex() const;

signals:
    void sliceDoubleClicked(Slice *pSlice);

protected:
     virtual void initializeGL();
     virtual void resizeGL(int w, int h);
     //virtual void paintGL();

     void mouseMoveEvent(QMouseEvent *event);
     void paintEvent(QPaintEvent *event);

     /* These events are used for implementing a hover effect */
     virtual void enterEvent(QEvent *event);
     virtual void leaveEvent(QEvent *event);

    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
     void drawDetails(QPainter *pPainter);
     void drawOutline(QPainter *pPainter);

    /* Whether we are inside a slice or not */
    bool weAreIn;

     /* Custom stuff */
     void png2raw(QString string);
     int texWidth;
     int texHeight;

     Magick::Image *pMagickImage;
     Magick::Blob blob;

     GLuint textureID;
     unsigned char *pRawPixel;

     Slice *pSlice;
};

#endif // MYGLWIDGET_H
