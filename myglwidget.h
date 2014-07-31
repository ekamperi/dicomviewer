#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <qt5/QtOpenGL/QGLWidget>

#include "Magick++.h"

class MyGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent = 0);
     ~MyGLWidget();

     void loadTextureFile(QString filename);

protected:
     virtual void initializeGL();
     virtual void resizeGL(int w, int h);
     virtual void paintGL();

     void mouseMoveEvent(QMouseEvent *event);

     void png2raw(QString string);
     int texWidth;
     int texHeight;

     Magick::Image *pMagickImage;
     Magick::Blob blob;

    GLuint textureID;
    unsigned char *rawPixel;
};

#endif // MYGLWIDGET_H
