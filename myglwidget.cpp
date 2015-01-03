#include "examdetails.h"
#include "myglwidget.h"

#include <iostream>
#include <QDebug>
#include <QtOpenGL/QGLWidget>

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    qDebug() << Q_FUNC_INFO;

    /* This is used to implement a hover like effect */
    this->weAreIn = false;

    pMagickImage = NULL;
}

MyGLWidget::~MyGLWidget()
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << this->pMagickImage;
    if (this->pMagickImage) {
        delete this->pMagickImage;
        this->pMagickImage = NULL;
    }
}

void MyGLWidget::initializeGL()
{
    qDebug() << Q_FUNC_INFO;
    QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
    qDebug() << "Driver Version String:" << versionString;
    qDebug() << "Current Context:" << format();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MyGLWidget::loadTextureFile(QString filename)
{
    qDebug() << Q_FUNC_INFO;
    this->makeCurrent();

    png2raw(filename);

    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Obtain an id for the texture */
    glGenTextures(1, &this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glBindTexture(GL_TEXTURE_2D, this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 this->texWidth,
                 this->texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 this->pRawPixel);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Set texture stretching parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glDisable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
}

void MyGLWidget::loadTextureFile2(unsigned char *pRawPixel,
                                  unsigned int width,
                                  unsigned int height,
                                  GLint format)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "width =" << width << "height =" << height << "format =" << format;

    // XXX
    this->pRawPixel = pRawPixel;

    /* From the QGLWiedget official documentation:
     *
     * If you need to call the standard OpenGL API functions from other places
     * (e.g. in your widget's constructor or in your own paint functions), you
     * must call makeCurrent() first.
     *
     * makeCurrent() makes this widget the current widget for OpenGL operations,
     * i.e. makes the widget's rendering context the current OpenGL rendering
     * context.
     */
    this->makeCurrent();

    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Obtain an id for the texture */
    glGenTextures(1, &this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Select texture */
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* XXX: Just a sanity check */
    Q_ASSERT((format == GL_LUMINANCE) || (format == GL_RGB));
    Q_ASSERT(this->pRawPixel);

    glTexImage2D(GL_TEXTURE_2D, 0, format,
                 width,
                 height, 0, format, GL_UNSIGNED_BYTE,
                 this->pRawPixel);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

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

        this->pRawPixel = (unsigned char *)this->blob.data();
        this->texWidth  = this->pMagickImage->columns();
        this->texHeight = this->pMagickImage->rows();
    } catch (Magick::Exception &error) {
        qDebug() << "Caught exception: " << error.what();
        this->pMagickImage = NULL;
    }
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
}

void MyGLWidget::paintEvent(QPaintEvent *event)
{
    qDebug() << Q_FUNC_INFO;

    this->makeCurrent();

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    /* Classical 3D drawing, usually performed by resizeGL */
    glViewport(0, 0, this->width(), this->height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);

    /* Classical 3D drawing, usually performed by paintGL() */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    /* Native drawing */
    QFont font = painter.font();
    font.setPointSize(7);
    painter.setFont(font);
    painter.setPen(Qt::red);
    painter.drawText(
                QRect(0, 0, this->width(), this->height()),
                Qt::TextWordWrap,
                  "Name: " + this->examDetails.getPatientName()
                + ", ID: " + this->examDetails.getPatientID()  + "\n"
                +  "Age: " + this->examDetails.getPatientAge() + "\n"
                +  "Sex: " + this->examDetails.getPatientSex() + "\n"
                + "Date: " + this->examDetails.getStudyDate());

    if (this->weAreIn) {
        QPen oldPen, myPen;
        oldPen = painter.pen();
        myPen.setWidth(6);
        myPen.setColor(Qt::red);
        painter.setPen(myPen);
        painter.drawRect(QRect(0, 0, this->width(), this->height()));
        painter.setPen(oldPen);
    }

    painter.end();
}

void MyGLWidget::setExamDetails(ExamDetails details)
{
    qDebug() << Q_FUNC_INFO;

    this->examDetails = details;
}

void MyGLWidget::enterEvent(QEvent * event)
{
    // qDebug() << Q_FUNC_INFO;

    this->weAreIn = true;
    this->update();

    QWidget::enterEvent(event);
}

void MyGLWidget::leaveEvent(QEvent * event)
{
    // qDebug() << Q_FUNC_INFO;

    this->weAreIn = false;
    this->update();

    QWidget::leaveEvent(event);
}
