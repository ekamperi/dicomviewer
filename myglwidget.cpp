#include "examdetails.h"
#include "slice.h"
#include "myglwidget.h"

#include <iostream>
#include <QDebug>
#include <QMouseEvent>
#include <QtGlobal>
#include <QtOpenGL/QGLWidget>

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    //pMagickImage = NULL;
}

MyGLWidget::~MyGLWidget()
{
//    if (this->pMagickImage) {
//        delete this->pMagickImage;
//        this->pMagickImage = NULL;
//    }
}

void MyGLWidget::setSlice(Slice *pSlice)
{
    Q_ASSERT(pSlice);

    this->pSlice = pSlice;
    this->loadTexture(
                pSlice->getRawPixelData(),
                pSlice->getWidth(),
                pSlice->getHeight(),
                pSlice->getFormat());
    this->update();
}

void MyGLWidget::loadTexture(Uint8 *pRawPixel,
                             unsigned int width,
                             unsigned int height,
                             GLint format)
{
    this->pRawPixel = pRawPixel;

    /* From the QGLWidget official documentation:
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

    glDeleteTextures(1, &this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    glEnable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Obtain an id for the texture */
    glGenTextures(1, &this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Select texture */
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    /* Just a sanity check */
    Q_ASSERT((format == GL_LUMINANCE) || (format == GL_RGB));
    Q_ASSERT(this->pRawPixel);

    glTexImage2D(GL_TEXTURE_2D, 0, format,
                 width,
                 height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
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
{/*
    try {
        pMagickImage = new Magick::Image(filename.toStdString().c_str());
        pMagickImage->write(&this->blob, "RGB");

        this->pRawPixel = (float*)this->blob.data();
        this->texWidth  = this->pMagickImage->columns();
        this->texHeight = this->pMagickImage->rows();
    } catch (Magick::Exception &error) {
        qDebug() << "Caught exception: " << error.what();
        this->pMagickImage = NULL;
    }*/
}

void MyGLWidget::initializeGL()
{
//    this->setAutoFillBackground(false);
//    this->setAutoBufferSwap(true);
//    this->setAttribute(Qt::WA_OpaquePaintEvent, true);
//    this->setAttribute(Qt::WA_NoSystemBackground, true);
}

void MyGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);

    QGLShaderProgram program(this);
    program.addShaderFromSourceFile(QGLShader::Vertex, "./vertex.sh");
    program.addShaderFromSourceFile(QGLShader::Fragment, "./fragment.sh");

    program.link();
    program.bind();

    // XXX
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0); glVertex2d(0.0, 0.0);
    glTexCoord2d(1.0, 0.0); glVertex2d(1.0, 0.0);
    glTexCoord2d(1.0, 1.0); glVertex2d(1.0, 1.0);
    glTexCoord2d(0.0, 1.0); glVertex2d(0.0, 1.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    this->drawDetails(&painter);
    painter.end();
}

void MyGLWidget::drawDetails(QPainter *pPainter)
{
    Q_ASSERT(pPainter);

    QFont font = pPainter->font();
    font.setPointSize(12);
    pPainter->setFont(font);
    pPainter->setPen(Qt::yellow);
    ExamDetails examDetails = this->pSlice->getExamDetails();

    pPainter->drawText(
                QRect(5, 5, this->width(), this->height()),
                Qt::TextWordWrap,
                  "Name: " + examDetails.getPatientName()
                + ", ID: " + examDetails.getPatientID()  + "\n"
                +  "Age: " + examDetails.getPatientAge() + "\n"
                +  "Sex: " + examDetails.getPatientSex() + "\n"
                + "Date: " + examDetails.getStudyDate());

}

void MyGLWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    unsigned int width = this->pSlice->getWidth();
    unsigned int height = this->pSlice->getHeight();
}

const Slice * MyGLWidget::getSlice() const
{
    Q_ASSERT(this->pSlice);
    return this->pSlice;
}

unsigned int MyGLWidget::getSliceIndex() const
{
    Q_ASSERT(this->pSlice);
    return this->pSlice->getIndex();
}
