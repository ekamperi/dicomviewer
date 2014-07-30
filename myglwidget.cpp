#include "myglwidget.h"

#include <iostream>
#include <QDebug>
#include <QtOpenGL/QGLWidget>

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    qDebug() << Q_FUNC_INFO;
}

MyGLWidget::~MyGLWidget()
{
    qDebug() << Q_FUNC_INFO;
}

void MyGLWidget::initializeGL()
{
    qDebug() << Q_FUNC_INFO;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MyGLWidget::resizeGL(int width, int height)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "width = " << width << "height = " << height;

    glViewport (0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::paintGL() {
    qDebug() << Q_FUNC_INFO;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 0.5, 0.5);

    glActiveTexture(GL_TEXTURE0);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glBindTexture(GL_TEXTURE_2D, this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void MyGLWidget::loadTextureFile(QString filename)
{
    qDebug() << Q_FUNC_INFO;

    png2raw(filename);

    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Obtain an id for the texture */
    glGenTextures(1, &this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glBindTexture(GL_TEXTURE_2D, this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    // Black/white checkerboard
    float pixels[] = {
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
//                 this->texWidth,
//                 this->texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
//                 this->rawPixel);
//    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Set texture stretching parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glDisable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
}

void MyGLWidget::png2raw(QString filename)
{
    qDebug() << Q_FUNC_INFO;

    try {
        pMagickImage = new Magick::Image(filename.toStdString().c_str());
        pMagickImage->write(&this->blob, "RGB");

        this->rawPixel = (unsigned char *)this->blob.data();
        this->texWidth  = this->pMagickImage->columns();
        this->texHeight = this->pMagickImage->rows();
    } catch (Magick::Exception &error) {
        qDebug() << "Caught exception: " << error.what();
    }
}
