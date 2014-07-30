#include "myglwidget.h"

#include <QDebug>
#include <QtOpenGL/QGLWidget>

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

void MyGLWidget::initializeGL()
{
    qDebug() << Q_FUNC_INFO;
    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
}

void MyGLWidget::resizeGL(int w, int h)
{
}

void MyGLWidget::paintGL() {
    qDebug() << Q_FUNC_INFO;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1, 0.5, 0.5);

    glBegin( GL_QUADS );
        glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
        glTexCoord2d(1.0,0.0); glVertex2d(1.0,0.0);
        glTexCoord2d(1.0,1.0); glVertex2d(1.0,1.0);
        glTexCoord2d(0.0,1.0); glVertex2d(0.0,1.0);
    glEnd();
}

void MyGLWidget::loadFile(QString filename)
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

    QImage texImg = QGLWidget::convertToGLFormat(img);
    if (texImg.isNull()) {
        qDebug() << "Error converting texture image to GLFormat";
        return;
    }

    glEnable(GL_TEXTURE_2D);

    /* Obtain an id for the texture */
    glGenTextures(1, &textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Set as the current texture */
    glBindTexture(GL_TEXTURE_2D, textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    qDebug() << "Texture width =" << texImg.width() << ", "
             << "Texture height =" << texImg.height();

    glTexImage2D(GL_TEXTURE_2D, 0, format,
                 texImg.width(),
                 texImg.height(), 0, format, GL_UNSIGNED_BYTE,
                 png2raw(filename));
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Set texture stretching parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
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
    textwidth  = w;
    textheight = h;

    unsigned char *myPixels = new unsigned char[3*w*h];

    Magick::PixelPacket *pp = img.getPixels(0, 0, w, h);
    for (int i = 0; i < w*h; i++) {
        myPixels[3*(i+1) - 3] = pp[i].red;
        myPixels[3*(i+1) - 2] = pp[i].green;
        myPixels[3*(i+1) - 1] = pp[i].blue;
    }

    return myPixels;
}
