#include "examdetails.h"
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
    QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
    qDebug() << "Driver Version String:" << versionString;
    qDebug() << "Current Context:" << format();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//void MyGLWidget::resizeGL(int width, int height)
//{
//    qDebug() << Q_FUNC_INFO;
//    qDebug() << "width = " << width << "height = " << height;

//    glViewport (0, 0, width, height);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0, 1, 1, 0, 0, 1);
//    glMatrixMode(GL_MODELVIEW);
//}

//void MyGLWidget::paintGL() {
//    qDebug() << Q_FUNC_INFO;

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glActiveTexture(GL_TEXTURE0);
//    Q_ASSERT(glGetError() == GL_NO_ERROR);

//    glBindTexture(GL_TEXTURE_2D, this->textureID);
//    Q_ASSERT(glGetError() == GL_NO_ERROR);

//    glEnable(GL_TEXTURE_2D);
//    glBegin(GL_QUADS);
//        glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
//        glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
//        glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
//        glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
//    glEnd();
//    glDisable(GL_TEXTURE_2D);
//}

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
                 this->rawPixel);
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

void MyGLWidget::loadTextureFile2(unsigned char *pRawPixel, unsigned int width, unsigned int height, GLint format)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "width =" << width << "height =" << height << "format =" << format;

    this->makeCurrent();

    this->rawPixel = pRawPixel;
    for (int i = 0; i < 200; i++)
        std::cout << (int)this->rawPixel[i] << " ";
    std::cout << std::endl << std::endl;

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

    glTexImage2D(GL_TEXTURE_2D, 0, format,
                 width,
                 height, 0, format, GL_UNSIGNED_BYTE,
                 this->rawPixel);
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

        this->rawPixel = (unsigned char *)this->blob.data();
        this->texWidth  = this->pMagickImage->columns();
        this->texHeight = this->pMagickImage->rows();
    } catch (Magick::Exception &error) {
        qDebug() << "Caught exception: " << error.what();
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
                "Name: "   + this->examDetails.getPatientName()
                + ", ID: " + this->examDetails.getPatientID() + "\n"
                + "Age: "  + this->examDetails.getPatientAge() + "\n"
                + "Sex: "  + this->examDetails.getPatientSex());

    painter.end();
}

void MyGLWidget::setExamDetails(ExamDetails details)
{
    qDebug() << Q_FUNC_INFO;

    this->examDetails = details;
}
