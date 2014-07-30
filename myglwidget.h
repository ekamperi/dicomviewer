#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QtOpenGL/QGLWidget>

#include "Magick++.h"

class MyGLWidget : public QGLWidget
{
    Q_OBJECT

public:
     MyGLWidget(QWidget *parent = 0);

     void loadFile(QString filename);

protected:
     void initializeGL();
     void resizeGL(int w, int h);
     void paintGL();

     unsigned char *png2raw(QString string);
     int textwidth;
     int textheight;

    GLuint textureID;
};

#endif // MYGLWIDGET_H
