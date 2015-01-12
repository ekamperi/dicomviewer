#include "examdetails.h"
#include "myglwidget.h"

#include <iostream>
#include <QDebug>
#include <QtGlobal>
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

    if (this->pMagickImage) {
        delete this->pMagickImage;
        this->pMagickImage = NULL;
    }
}

void MyGLWidget::setSlice(Slice *pSlice)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(pSlice);

    this->pSlice = pSlice;
    this->loadTexture(
                pSlice->getRawPixelData(),
                pSlice->getWidth(),
                pSlice->getHeight(),
                pSlice->getFormat());
    this->update();
}

void MyGLWidget::loadTexture(unsigned char *pRawPixel,
                             unsigned int width,
                             unsigned int height,
                             GLint format)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "width =" << width << "height =" << height << "format =" << format;

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
    // qDebug() << Q_FUNC_INFO;
}


void MyGLWidget::initializeGL()
{
    qDebug() << Q_FUNC_INFO;
//    QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
//    qDebug() << "Driver Version String:" << versionString;
//    qDebug() << "Current Context:" << format();

    this->setAutoFillBackground(false);
    this->setAutoBufferSwap(false);
    this->setAttribute(Qt::WA_OpaquePaintEvent, true);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
}

void MyGLWidget::resizeGL(int w, int h)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "width =" << w << "height =" << h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::paintEvent(QPaintEvent *event)
{
    qDebug() << Q_FUNC_INFO;

    QPainter painter;
    painter.begin(this);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

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
    Q_ASSERT(glGetError() == GL_NO_ERROR);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    this->drawDetails(&painter);
    this->drawOutline(&painter);    
    painter.end();
    swapBuffers();
}

void MyGLWidget::drawDetails(QPainter *pPainter)
{
    Q_ASSERT(pPainter);

    QFont font = pPainter->font();
    font.setPointSize(7);
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

void MyGLWidget::drawOutline(QPainter *pPainter)
{
    qDebug() << Q_FUNC_INFO << this->width() << this->height();
    Q_ASSERT(pPainter);

    if (this->weAreIn || this->pSlice->isSelected()) {
        QPen oldPen, myPen;
        oldPen = pPainter->pen();
        myPen.setWidth(4);
        myPen.setColor(Qt::red);
        pPainter->setPen(myPen);
        pPainter->drawRect(QRect(0, 0, this->width(), this->height()));
        pPainter->setPen(oldPen);
    }
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

void MyGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;

    emit sliceDoubleClicked(this->pSlice);
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
