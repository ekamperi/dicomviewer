#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <qt5/QtOpenGL/QGLWidget>

#include "examdetails.h"
#include "Magick++.h"

class MyGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent = 0);
     ~MyGLWidget();

     void loadTextureFile(QString filename);
     void loadTextureFile2(unsigned char *pRawData, unsigned int width, unsigned int height, GLint format);

     void setExamDetails(ExamDetails details);


protected:
     virtual void initializeGL();
     //virtual void resizeGL(int w, int h);
     //virtual void paintGL();

     void mouseMoveEvent(QMouseEvent *event);
     void paintEvent(QPaintEvent *event);

     void png2raw(QString string);
     int texWidth;
     int texHeight;

     Magick::Image *pMagickImage;
     Magick::Blob blob;

     GLuint textureID;
     unsigned char *rawPixel;
     ExamDetails examDetails;
};

#endif // MYGLWIDGET_H
