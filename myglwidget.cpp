#include "myglwidget.h"

#include <iostream>
#include <QDebug>
#include <QtOpenGL/QGLWidget>

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

void MyGLWidget::initializeGL()
{
    qDebug() << Q_FUNC_INFO;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0, 0, 0.5);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
}

void MyGLWidget::resizeGL(int w, int h)
{
}

void MyGLWidget::paintGL() {
    qDebug() << Q_FUNC_INFO;

    glBindTexture(GL_TEXTURE_2D, textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glActiveTexture(textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glEnable(GL_TEXTURE_2D);

    glColor3f(1, 0.5, 0.5);

    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
        glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
        glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glActiveTexture(0);
}

void MyGLWidget::loadTextureFile(QString filename)
{
    qDebug() << Q_FUNC_INFO;

    QImage img;
    if (!img.load(filename)) {
        qDebug("Image not found");
        return;
    }

    GLenum format;
    if (img.hasAlphaChannel()) {
        format = GL_RGBA;
    } else {
        format = GL_RGB;
    }
    format = GL_RGB;

    QImage texImg = QGLWidget::convertToGLFormat(img);
    if (texImg.isNull()) {
        qDebug() << "Error converting texture image to GLFormat";
        return;
    }
    this->rawPixel = png2raw(filename); // deep copy
    this->textheight = texImg.height();
    this->textwidth = texImg.width();

    glEnable(GL_TEXTURE_2D);

    /* Obtain an id for the texture */
    glGenTextures(1, &textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Set texture stretching parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    qDebug() << "About to call glTexImage2D()" << this->textwidth << this->textheight;
    glTexImage2D(GL_TEXTURE_2D, 0, format,
                 this->textwidth,
                 this->textheight, 0, format, GL_UNSIGNED_BYTE,
                 this->rawPixel);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
    glDisable(GL_TEXTURE_2D);
}

/* This is (should be) equivalent to QImage::bits() */
/* XXX: Caller must free up the memory */
unsigned char *MyGLWidget::png2raw(QString filename)
{
    qDebug() << Q_FUNC_INFO;
    Magick::Image img;

    try {
        img.read(filename.toStdString().c_str());
    } catch (Magick::Exception &error) {
        qDebug() << "Caught exception: " << error.what();
    }

    int w = img.size().width();
    int h = img.size().height();
    this->textwidth  = w;
    this->textheight = h;
    qDebug() << w << h;

    unsigned char *myPixels = new unsigned char[3*w*h];

    // RGB RGB RGB RGB RGB
    Magick::PixelPacket *pp = img.getPixels(0, 0, w, h);
    for (int i = 0; i < w*h; i++) {
        myPixels[3*(i+1) - 3] = pp[i].red;
        myPixels[3*(i+1) - 2] = pp[i].green + 100;
        myPixels[3*(i+1) - 1] = pp[i].blue;
    }
    return myPixels;
}
